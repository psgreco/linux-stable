// SPDX-License-Identifier: GPL-2.0
/*
 * Freescale Management Complex (MC) bus driver MSI support
 *
 * Copyright (C) 2015-2016 Freescale Semiconductor, Inc.
 * Author: German Rivera <German.Rivera@freescale.com>
 * Copyright 2019 NXP
 *
 */

#include <linux/acpi_iort.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/irq.h>
#include <linux/msi.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/fsl/mc.h>

static struct irq_chip its_msi_irq_chip = {
	.name = "ITS-fMSI",
	.irq_mask = irq_chip_mask_parent,
	.irq_unmask = irq_chip_unmask_parent,
	.irq_eoi = irq_chip_eoi_parent,
	.irq_set_affinity = msi_domain_set_affinity
};

static int its_fsl_mc_msi_prepare(struct irq_domain *msi_domain,
				  struct device *dev,
				  int nvec, msi_alloc_info_t *info)
{
	struct fsl_mc_device *mc_bus_dev;
	struct msi_domain_info *msi_info;

	if (!dev_is_fsl_mc(dev))
		return -EINVAL;

	mc_bus_dev = to_fsl_mc_device(dev);
	if (!(mc_bus_dev->flags & FSL_MC_IS_DPRC))
		return -EINVAL;

	/*
	 * Set the device Id to be passed to the GIC-ITS:
	 *
	 * NOTE: This device id corresponds to the IOMMU stream ID
	 * associated with the DPRC object (ICID).
	 */
	info->scratchpad[0].ul = mc_bus_dev->icid;
	msi_info = msi_get_domain_info(msi_domain->parent);

	/* Allocate at least 32 MSIs, and always as a power of 2 */
	nvec = max_t(int, 32, roundup_pow_of_two(nvec));
	return msi_info->ops->msi_prepare(msi_domain->parent, dev, nvec, info);
}

static struct msi_domain_ops its_fsl_mc_msi_ops __ro_after_init = {
	.msi_prepare = its_fsl_mc_msi_prepare,
};

static struct msi_domain_info its_fsl_mc_msi_domain_info = {
	.flags	= (MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS),
	.ops	= &its_fsl_mc_msi_ops,
	.chip	= &its_msi_irq_chip,
};

static const struct of_device_id its_device_id[] = {
	{	.compatible	= "arm,gic-v3-its",	},
	{},
};

static int __init its_fsl_mc_msi_init_one(struct fwnode_handle *handle,
					const char *name)
{
	struct irq_domain *parent;
	struct irq_domain *mc_msi_domain;

	parent = irq_find_matching_fwnode(handle, DOMAIN_BUS_NEXUS);
	if (!parent || !msi_get_domain_info(parent)) {
		pr_err("%s: Unable to locate ITS domain\n", name);
		return -ENXIO;
	}

	mc_msi_domain = fsl_mc_msi_create_irq_domain(
					 handle,
					 &its_fsl_mc_msi_domain_info,
					 parent);
	if (!mc_msi_domain)
		pr_err("ACPIF: unable to create fsl-mc domain\n");

	pr_info("fsl-mc MSI: domain created\n");

	return 0;
}

static int __init
its_fsl_mc_msi_parse_madt(struct acpi_subtable_header *header,
			const unsigned long end)
{
	struct acpi_madt_generic_translator *its_entry;
	struct fwnode_handle *dom_handle;
	const char *node_name;
	int err = -ENXIO;

	its_entry = (struct acpi_madt_generic_translator *)header;
	node_name = kasprintf(GFP_KERNEL, "ITS@0x%lx",
				(long)its_entry->base_address);

	dom_handle = iort_find_domain_token(its_entry->translation_id);
	if (!dom_handle) {
		pr_err("%s: Unable to locate ITS domain handle\n", node_name);
		goto out;
	}

	err = its_fsl_mc_msi_init_one(dom_handle, node_name);
	if (!err)
		pr_info("fsl-mc MSI: %s domain created\n", node_name);

out:
	kfree(node_name);
	return err;
}

static int __init its_fsl_mc_acpi_msi_init(void)
{
	acpi_table_parse_madt(ACPI_MADT_TYPE_GENERIC_TRANSLATOR,
				its_fsl_mc_msi_parse_madt, 0);
	return 0;
}

static int __init its_fsl_mc_of_msi_init(void)
{
	struct device_node *np;
	struct irq_domain *parent;
	struct irq_domain *mc_msi_domain;

	for (np = of_find_matching_node(NULL, its_device_id); np;
	     np = of_find_matching_node(np, its_device_id)) {
		if (!of_device_is_available(np))
			continue;
		if (!of_property_read_bool(np, "msi-controller"))
			continue;

		parent = irq_find_matching_host(np, DOMAIN_BUS_NEXUS);
		if (!parent || !msi_get_domain_info(parent)) {
			pr_err("%pOF: unable to locate ITS domain\n", np);
			continue;
		}

		mc_msi_domain = fsl_mc_msi_create_irq_domain(
						 of_node_to_fwnode(np),
						 &its_fsl_mc_msi_domain_info,
						 parent);
		if (!mc_msi_domain) {
			pr_err("%pOF: unable to create fsl-mc domain\n", np);
			continue;
		}

		pr_info("fsl-mc MSI: %pOF domain created\n", np);
	}
	return 0;
}

static int __init its_fsl_mc_msi_init(void)
{
	its_fsl_mc_of_msi_init();
	its_fsl_mc_acpi_msi_init();

	return 0;
}
early_initcall(its_fsl_mc_msi_init);
