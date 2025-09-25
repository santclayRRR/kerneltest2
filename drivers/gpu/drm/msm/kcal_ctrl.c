// SPDX-License-Identifier: GPL-2.0
// KCAL Driver for Kernel Color Control

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static int kcal_enable = 1;
static int kcal_r = 256, kcal_g = 256, kcal_b = 256;
static int kcal_min = 35;
static int kcal_sat = 255, kcal_hue = 0, kcal_val = 255, kcal_cont = 255;

static ssize_t kcal_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    return scnprintf(buf, PAGE_SIZE, "%d %d %d\n", kcal_r, kcal_g, kcal_b);
}

static ssize_t kcal_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
    int r, g, b;
    if (sscanf(buf, "%d %d %d", &r, &g, &b) == 3) {
        kcal_r = r;
        kcal_g = g;
        kcal_b = b;
    }
    return count;
}

static DEVICE_ATTR_RW(kcal);

static ssize_t kcal_enable_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    return scnprintf(buf, PAGE_SIZE, "%d\n", kcal_enable);
}

static ssize_t kcal_enable_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
    int enable;
    if (kstrtoint(buf, 0, &enable) == 0)
        kcal_enable = enable;
    return count;
}

static DEVICE_ATTR_RW(kcal_enable);

static struct attribute *kcal_attrs[] = {
    &dev_attr_kcal.attr,
    &dev_attr_kcal_enable.attr,
    NULL,
};

static struct attribute_group kcal_attr_group = {
    .attrs = kcal_attrs,
};

static int kcal_probe(struct platform_device *pdev)
{
    sysfs_create_group(&pdev->dev.kobj, &kcal_attr_group);
    pr_info("KCAL: probe success\n");
    return 0;
}

static int kcal_remove(struct platform_device *pdev)
{
    sysfs_remove_group(&pdev->dev.kobj, &kcal_attr_group);
    pr_info("KCAL: removed\n");
    return 0;
}

static struct platform_driver kcal_driver = {
    .driver = {
        .name = "kcal_ctrl",
    },
    .probe = kcal_probe,
    .remove = kcal_remove,
};

static struct platform_device *kcal_device;

static int __init kcal_init(void)
{
    int ret;
    kcal_device = platform_device_register_simple("kcal_ctrl", -1, NULL, 0);
    if (IS_ERR(kcal_device))
        return PTR_ERR(kcal_device);

    ret = platform_driver_register(&kcal_driver);
    if (ret) {
        platform_device_unregister(kcal_device);
        return ret;
    }
    pr_info("KCAL: initialized\n");
    return 0;
}

static void __exit kcal_exit(void)
{
    platform_driver_unregister(&kcal_driver);
    platform_device_unregister(kcal_device);
    pr_info("KCAL: exited\n");
}

module_init(kcal_init);
module_exit(kcal_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("KCAL Port");
MODULE_DESCRIPTION("Kernel Color Control");