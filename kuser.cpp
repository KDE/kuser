#include "kuser.h"
#include "misc.h"

KUser::KUser() {
  p_name.setStr("");
  p_pwd.setStr("*");
  p_dir.setStr("");
  p_shell.setStr("");
  p_fname.setStr("");
  p_office1.setStr("");
  p_office2.setStr("");
  p_address.setStr("");
  p_uid     = first_free();
  p_gid     = 100;

#ifdef _KU_SHADOW   
  s_pwd.setStr("");
  s_lstchg  = 0;
  s_min     = 0;
  s_max     = 99999;
  s_warn    = 0;
  s_inact   = 99999;
  s_expire  = 99999;
  s_flag    = 0;
#endif

#ifdef _KU_QUOTA
  if (is_quota == 1) {
  }
#endif
}
  
KUser::KUser(KUser *copy) {
    p_name    = copy->p_name;
    p_pwd     = copy->p_pwd;
    p_dir     = copy->p_dir;
    p_shell   = copy->p_shell;
    p_fname   = copy->p_fname;
    p_office1 = copy->p_office1;
    p_office2 = copy->p_office2;
    p_address = copy->p_address;
    p_uid     = copy->p_uid;
    p_gid     = copy->p_gid;

#ifdef _KU_SHADOW   
  if (is_shadow == 1) {
    s_pwd     = copy->s_pwd;
    s_lstchg  = copy->s_lstchg;
    s_min     = copy->s_min;
    s_max     = copy->s_max;
    s_warn    = copy->s_warn;
    s_inact   = copy->s_inact;
    s_expire  = copy->s_expire;
    s_flag    = copy->s_flag;
  }
#endif

#ifdef _KU_QUOTA
  if (is_quota == 1) {
    quota = copy->quota;
  }
#endif 
}

KUser::~KUser() {
}

