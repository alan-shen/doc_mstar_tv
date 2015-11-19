===================================================================================================
drivers/usb/gadget/android.c:
+++++++++++++++++++++++++++++

static struct usb_string strings_dev[] = {
    [STRING_MANUFACTURER_IDX].s = manufacturer_string,
    [STRING_PRODUCT_IDX].s = product_string,
    [STRING_SERIAL_IDX].s = serial_string,
    {  }            /* end of list */
};

static struct usb_gadget_strings stringtab_dev = {
    .language   = 0x0409,   /* en-us */
    .strings    = strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
    &stringtab_dev,
    NULL,
};

static struct usb_device_descriptor device_desc = {
    .bLength              = sizeof(device_desc),
    .bDescriptorType      = USB_DT_DEVICE,
#ifdef CONFIG_USB_MU3D_DRV
    .bcdUSB               = __constant_cpu_to_le16(0x0300),
#else
    .bcdUSB               = __constant_cpu_to_le16(0x0200),
#endif
    .bDeviceClass         = USB_CLASS_PER_INTERFACE,
    .idVendor             = __constant_cpu_to_le16(VENDOR_ID),
    .idProduct            = __constant_cpu_to_le16(PRODUCT_ID),
    .bcdDevice            = __constant_cpu_to_le16(0xffff),
    .bNumConfigurations   = 1,
};

static struct usb_composite_driver android_usb_driver = {
    .name       = "android_usb",
    .dev        = &device_desc,
    .strings    = dev_strings,
    .bind       = android_bind,
    .unbind     = android_usb_unbind,
    .disconnect = android_disconnect,
#ifdef CONFIG_USB_MU3D_DRV
    .max_speed  = USB_SPEED_SUPER
#else
    .max_speed  = USB_SPEED_HIGH
#endif
};

static int __init init(void)
{
	dev->functions = supported_functions;
	usb_composite_probe(&android_usb_driver);
}

late_initcall(init);

===================================================================================================
static struct android_usb_function ffs_function = {          
    .name       = "ffs",                                     
    .init       = ffs_function_init,
    .enable     = ffs_function_enable,
    .disable    = ffs_function_disable,
    .cleanup    = ffs_function_cleanup,
    .bind_config    = ffs_function_bind_config,
    .attributes = ffs_function_attributes,
};
static struct android_usb_function adb_function = {
    .name       = "adb",
    .enable     = adb_android_function_enable,
    .disable    = adb_android_function_disable,
    .init       = adb_function_init,
    .cleanup    = adb_function_cleanup,
    .bind_config    = adb_function_bind_config,
};
static struct android_usb_function acm_function = {
    .name       = "acm",
    .init       = acm_function_init,
    .cleanup    = acm_function_cleanup,
    .bind_config    = acm_function_bind_config,
    .unbind_config  = acm_function_unbind_config,
    .attributes = acm_function_attributes,
};
static struct android_usb_function mtp_function = {
    .name       = "mtp",
    .init       = mtp_function_init,
    .cleanup    = mtp_function_cleanup,
    .bind_config    = mtp_function_bind_config,
    .ctrlrequest    = mtp_function_ctrlrequest,
};
static struct android_usb_function ptp_function = {
    .name       = "ptp",
    .init       = ptp_function_init,
    .cleanup    = ptp_function_cleanup,
    .bind_config    = ptp_function_bind_config,
};
static struct android_usb_function rndis_function = {
    .name       = "rndis",
    .init       = rndis_function_init,
    .cleanup    = rndis_function_cleanup,
    .bind_config    = rndis_function_bind_config,
    .unbind_config  = rndis_function_unbind_config,
    .attributes = rndis_function_attributes,
};
static struct android_usb_function mass_storage_function = {
    .name       = "mass_storage",
    .init       = mass_storage_function_init,
    .cleanup    = mass_storage_function_cleanup,
    .bind_config    = mass_storage_function_bind_config,
    .attributes = mass_storage_function_attributes,
};
static struct android_usb_function accessory_function = {
    .name       = "accessory",
    .init       = accessory_function_init,
    .cleanup    = accessory_function_cleanup,
    .bind_config    = accessory_function_bind_config,
    .ctrlrequest    = accessory_function_ctrlrequest,
};
static struct android_usb_function audio_source_function = {
    .name       = "audio_source",
    .init       = audio_source_function_init,
    .cleanup    = audio_source_function_cleanup,
    .bind_config    = audio_source_function_bind_config,
    .unbind_config  = audio_source_function_unbind_config,
    .attributes = audio_source_function_attributes,
};
===================================================================================================

static struct android_usb_function *supported_functions[] = {                                                                                                                 
    &ffs_function,                                                                                                                                                            
    &adb_function,                                                                                                                                                            
    &acm_function,                                                                                                                                                            
    &mtp_function,                                                                                                                                                            
    &ptp_function,                                                                                                                                                            
    &rndis_function,                                                                                                                                                          
    &mass_storage_function,                                                                                                                                                   
    &accessory_function,                                                                                                                                                      
    &audio_source_function,                                                                                                                                                   
    NULL                                                                                                                                                                      
};

static int android_init_functions(struct android_usb_function **functions, struct usb_composite_dev *cdev)
{
	for (; (f = *functions++); index++) {
		device_create(android_class, dev->dev, MKDEV(0, index), f, f->dev_name);
        if (f->init) {
            err = f->init(f, cdev);
		...
		device_create_file(f->dev, attr);
	}
}

static int android_bind(struct usb_composite_dev *cdev)
{
	android_init_functions(dev->functions, cdev);
}

static int __init init(void)
{
	dev->functions = supported_functions;
}
===================================================================================================
