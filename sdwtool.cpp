#include <kmsgbox.h>
#include "includes.h"
#include "misc.h"
#include "sdwtool.h"

int s_saved = 0;

#ifdef _KU_SHADOW
void sdw_read(void)
{
  FILE *f;
  struct spwd *spw;
  KUser *up = NULL;

  if (is_shadow) {
    if ((f = fopen(SHADOW_FILE, "r")) == NULL) {
      is_shadow = 0; 
      sprintf(tmp, _("Error opening %s"), SHADOW_FILE);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      return;
    }

    while ((spw = fgetspent(f)))      // read a shadow password structure
    {
      if ((up = user_lookup(spw->sp_namp)) != NULL)
        shadow_cp(up, spw);             // cp the shadow info
      else {
        sprintf(tmp, _("No /etc/passwd entry for %s.\n\
Entry will be removed at the next `Save'-operation."), spw->sp_namp);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      }
    }

    fclose(f);
  }
}

void sdw_write()
{
  FILE *f;
  spwd *spwp;
  spwd s;
  KUser *up;


  if (is_shadow) {
    if (!s_saved) {
      backup(SHADOW_FILE);
      s_saved = TRUE;
    }

    if ((f = fopen(SHADOW_FILE, "w")) == NULL) {
      sprintf(tmp, _("Error opening %s for writing"), SHADOW_FILE);
      KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
    }

    s.sp_namp = (char *)malloc(200);
    s.sp_pwdp = (char *)malloc(200);
    
    for (uint index = 0; index < users.count(); index++)
    {
      up = users.at(index);
      if (!(const char *)up->s_pwd)
      {
        sprintf(tmp, _("No shadow entry for %s."), (const char *)up->p_name);
        KMsgBox::message(0, _("Error"), tmp, KMsgBox::STOP);
      }
      else {
        strcpy(s.sp_namp, (const char *)up->p_name);
        strcpy(s.sp_pwdp, (const char *)up->s_pwd);
        s.sp_lstchg = up->s_lstchg;
        s.sp_min    = up->s_min;
        s.sp_max    = up->s_max;
        s.sp_warn   = up->s_warn;
        s.sp_inact  = up->s_inact;
        s.sp_expire = up->s_expire;
        s.sp_flag   = up->s_flag;

        spwp = &s;
        putspent(spwp, f);
      }
    }
    fclose(f);

    chmod(SHADOW_FILE, SHADOW_FILE_MASK);

  }

  free(s.sp_namp);
  free(s.sp_pwdp);
}

void shadow_cp(KUser *up, const spwd *src)
{
  up->s_pwd.setStr(src->sp_pwdp);        // cp the encrypted pwd

  up->s_lstchg   = src->sp_lstchg;
  up->s_min      = src->sp_min;
  up->s_max      = src->sp_max;
  up->s_warn     = src->sp_warn;
  up->s_inact    = src->sp_inact;
  up->s_expire   = src->sp_expire;
  up->s_flag     = src->sp_flag;
}

#endif // _KU_SHADOW

