===================================================================================================
[struct usb_gadget_driver   ]	->
[struct usb_composite_driver]	->	usb_composite_probe()
===================================================================================================
===================================================================================================

struct android_dev {
    struct android_usb_function **functions;			// "supported_functions[]"
			---------------------------------------------------------------------------------------
			struct android_usb_function {                                                                                                                                                 
			    char *name;
			    void *config;                                                                                                                                                             
			    struct device *dev;                                                                                                                                                       
			    char *dev_name;
			    struct device_attribute **attributes;                                                                                                                                     
				struct list_head enabled_list;						// functions_store()->android_enable_function()->加入到链表enabled_functions中

			    int  (*init)         (struct android_usb_function *, struct usb_composite_dev *);                                                                                                   
			    void (*cleanup)      (struct android_usb_function *);
			    void (*enable)       (struct android_usb_function *);                                                                                                                            
			    void (*disable)      (struct android_usb_function *);                                                                                                                           
			    int  (*bind_config)  (struct android_usb_function *, struct usb_configuration *);
			    void (*unbind_config)(struct android_usb_function *, struct usb_configuration *);
				int  (*ctrlrequest)  (struct android_usb_function *, struct usb_composite_dev *, const struct usb_ctrlrequest *);
			};
			---------------------------------------------------------------------------------------
    struct list_head enabled_functions;
    struct usb_composite_dev *cdev;
			------------------------------------------------------------
			static struct usb_composite_driver android_usb_driver = {
			    .name       = "android_usb",
			    .dev        = &device_desc,
			    .strings    = dev_strings,
			    .bind       = android_bind,
			    .unbind     = android_usb_unbind,
			    .disconnect = android_disconnect,
			    .max_speed  = USB_SPEED_SUPER
				// add in usb_composite_probe()
				.gadget_driver = composite_driver_template;
								--------------------------------------------------------------------------------------------
								static const struct usb_gadget_driver composite_driver_template = {
									    .bind       = composite_bind,
									    .unbind     = composite_unbind,
									    .setup      = composite_setup,
									    .disconnect = composite_disconnect,
									    .suspend    = composite_suspend,
									    .resume     = composite_resume,
									    .driver = {
									        .owner      = THIS_MODULE,
											// add in usb_composite_probe()   
											.name       = usb_composite_driver.driver->name;				// "android_usb"
									    },
										// add in usb_composite_probe()   
										.function   =  (char *) driver->name;			// "android_usb"
										.max_speed  = driver->max_speed;				// USB_SPEED_SUPER
								};
								--------------------------------------------------------------------------------------------
			};
			------------------------------------------------------------
    struct device *dev;

    bool enabled;			// enable_store() 中设定, 表示usb设备是否使能.
    int disable_depth;
    struct mutex mutex;
    bool connected;
    bool sw_connected;
    struct work_struct work;
    char ffs_aliases[256];
    int rezero_cmd;
};

===================================================================================================
===================================================================================================
