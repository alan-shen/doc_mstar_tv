=======================================================================================
frameworks/base/services/core/java/com/android/server/MasterClearReceiver.java
------------------------------------------------------------------------------

 28 public class MasterClearReceiver extends BroadcastReceiver {                                                                                               
 29     private static final String TAG = "MasterClear";                                                                                                       
 30     
 31     @Override
 32     public void onReceive(final Context context, final Intent intent) {                                                                                    
 33         if (intent.getAction().equals(Intent.ACTION_REMOTE_INTENT)) {
 34             if (!"google.com".equals(intent.getStringExtra("from"))) {
 35                 Slog.w(TAG, "Ignoring master clear request -- not from trusted server.");                                                                  
 36                 return;                                                                                                                                    
 37             }                                                                                                                                              
 38         }                                                                                                                                                  
 39         
 40         //MITV add begin recovery 
 41         final boolean wipeAllData = intent.getBooleanExtra("com.xiaomi.tv.WIPE_ALL_DATA",false);
 42         final boolean wipeInstalledApps = intent.getBooleanExtra("com.xiaomi.tv.WIPE_INSTALLED_APPS",false);                                               
 43         //MITV add end                                                                                                                                     
 44         
 45         final boolean shutdown = intent.getBooleanExtra("shutdown", false);                                                                                
 46         final String reason = intent.getStringExtra(Intent.EXTRA_REASON);                                                                                  
 47         
 48         Slog.w(TAG, "!!! FACTORY RESET !!!");
 49         // The reboot call is blocking, so we need to do it on another thread.                                                                             
 50         Thread thr = new Thread("Reboot") {                                                                                                                
 51             @Override
 52             public void run() {                                                                                                                            
 53                 try {
 54                     //MITV add recovery
 55                     RecoverySystem.setMiClearOption(wipeAllData,wipeInstalledApps);                                                                        
 56                     //MITV add end
 57                     RecoverySystem.rebootWipeUserData(context, shutdown, reason);                                                                          
 58                     Log.wtf(TAG, "Still running after master clear?!");                                                                                    
 59                 } catch (IOException e) {
 60                     Slog.e(TAG, "Can't perform master clear/factory reset", e);                                                                            
 61                 } catch (SecurityException e) {
 62                     Slog.e(TAG, "Can't perform master clear/factory reset", e);                                                                            
 63                 }                                                                                                                                          
 64             }                                                                                                                                              
 65         }
 66         thr.start();                                                                                                                                       
 67     }                                                                                                                                                      
 68 }

=======================================================================================
frameworks/base/core/java/android/os/RecoverySystem.java
--------------------------------------------------------

429         // Block until the ordered broadcast has completed.                                                                                                
430         condition.block();                                                                                                                                 
431                                                                                                                                                            
432         String shutdownArg = null;                                                                                                                         
433         if (shutdown) {                                                                                                                                    
434             shutdownArg = "--shutdown_after";                                                                                                              
435         }                                                                                                                                                  
436                                                                                                                                                            
437         String reasonArg = null;                                                                                                                           
438         if (!TextUtils.isEmpty(reason)) {                                                                                                                  
439             reasonArg = "--reason=" + sanitizeArg(reason);                                                                                                 
440         }                                                                                                                                                  
441                                                                                                                                                            
442         final String localeArg = "--locale=" + Locale.getDefault().toString();                                                                             
443                                                                                                                                                            
444         //MITV add recovery                                                                                                                                
445         if (mMiClearDataOption == 0) {                                                                                                                     
446             bootCommand(context, shutdownArg, "--wipe_data", reasonArg, localeArg);                                                                        
447         } else if (mMiClearDataOption == 1) {                                                                                                              
448             bootCommand(context, shutdownArg, "--wipe_data", reasonArg, localeArg, "--clean_app");                                                         
449         } else if (mMiClearDataOption == 2) {                                                                                                              
450             bootCommand(context, "--format_data");                                                                                                         
451         }                                                                                                                                                  
452         //MITV add end                                                                                                                                     
453     }

=======================================================================================
bootable/recovery/recovery.cpp
-------------------------------

static const struct option OPTIONS[] = {
 101   { "send_intent", required_argument, NULL, 's' }, 
 102   { "update_package", required_argument, NULL, 'u' },                                                                                                     
 103   { "wipe_data", no_argument, NULL, 'w' },
 104   { "wipe_cache", no_argument, NULL, 'c' },
 105   { "format_data", no_argument, NULL, 'f' },                                                                                                              
 106   { "clean_app", no_argument, NULL, 'n' },                                                                                                                
 107   { "show_text", no_argument, NULL, 't' }, 
 108   { "graphic_mode", no_argument, NULL, 'G' },                                                                                                             
 109   { "just_exit", no_argument, NULL, 'x' }, 
 110   { "locale", required_argument, NULL, 'l' },                                                                                                             
 111 //#ifdef SUPPORT_FOTA
 112   { "fota_delta_path", required_argument, NULL, 'f' },
 113   //{ "fota_delta_path_1", required_argument, NULL, 'g' },                                                                                                
 114 //#endif
 115 #ifdef SUPPORT_DATA_BACKUP_RESTORE //wschen 2011-05-16                                                                                                    
 116   { "restore_data", required_argument, NULL, 'd' },                                                                                                       
 117 #endif //SUPPORT_DATA_BACKUP_RESTORE
 118   { "stages", required_argument, NULL, 'g' },
 119   { "shutdown_after", no_argument, NULL, 'p' },                                                                                                           
 120   { "reason", required_argument, NULL, 'r' },                                                                                                             
 121   { "restore", no_argument, NULL, 'm' },                                                                                                                  
 122   { NULL, 0, NULL, 0 },                                                                                                                                   
 123 };

======================================================================================
