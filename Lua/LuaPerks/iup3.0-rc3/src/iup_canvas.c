/** \file
 * \brief Canvas Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_canvas.h"


void iupCanvasCalcScrollIntPos(double min, double max, double page, double pos, 
                                 int imin,   int imax,  int *ipage,  int *ipos)
{
  double range = max-min;
  int irange = imax-imin;
  double ratio = ((double)irange)/range;

  *ipage = (int)(page*ratio);
  if (*ipage > irange) *ipage = irange;
  if (*ipage < 1) *ipage = 1;

  if (ipos)
  {
    *ipos = (int)((pos-min)*ratio) + imin;
    if (*ipos < imin) *ipos = imin;
    if (*ipos > (imax - *ipage)) *ipos = imax - *ipage;
  }
}

void iupCanvasCalcScrollRealPos(double min, double max, double *pos, 
                                 int imin,   int imax,  int ipage,  int *ipos)
{
  double range = max-min;
  int irange = imax-imin;
  double ratio = ((double)irange)/range;

  if (*ipos < imin) *ipos = imin;
  if (*ipos > (imax - ipage)) *ipos = imax - ipage;

  *pos = min + ((double)(*ipos-imin))/ratio;
}

char* iupCanvasGetPosXAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%f", ih->data->posx);
  return str;
}

char* iupCanvasGetPosYAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%f", ih->data->posy);
  return str;
}

static int iCanvasCreateMethod(Ihandle* ih, void** params)
{
  if (params && params[0])
  {
    char* action = (char*)params[0];
    iupAttribStoreStr(ih, "ACTION", action);
  }

  ih->data = iupALLOCCTRLDATA();

  /* default EXPAND is YES */
  ih->expand = IUP_EXPAND_BOTH;
  
  return IUP_NOERROR;
}

static void iCanvasComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w = 0, natural_h = 0;
  (void)expand; /* unset if not a container */

  /* canvas natural size is 1 character */
  iupdrvFontGetCharSize(ih, &natural_w, &natural_h);

  *w = natural_w;
  *h = natural_h;
}


/******************************************************************************/


Ihandle* IupCanvas(const char* action)
{
  void *params[2];
  params[0] = (void*)action;
  params[1] = NULL;
  return IupCreatev("canvas", params);
}

Iclass* iupCanvasGetClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "canvas";
  ic->format = "A"; /* one optional callback name */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->Create = iCanvasCreateMethod;
  ic->ComputeNaturalSize = iCanvasComputeNaturalSizeMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "DROPFILES_CB", "siii");
  iupClassRegisterCallback(ic, "RESIZE_CB", "ii");
  iupClassRegisterCallback(ic, "FOCUS_CB", "i");
  iupClassRegisterCallback(ic, "WOM_CB", "i");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "KEYPRESS_CB", "ii");
  iupClassRegisterCallback(ic, "ACTION", "ff");
  iupClassRegisterCallback(ic, "SCROLL_CB", "iff");
  iupClassRegisterCallback(ic, "WHEEL_CB", "fiis");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  {
    /* Change the default to YES */
    IattribGetFunc _get;
    IattribSetFunc _set;
    iupClassRegisterGetAttribute(ic, "EXPAND", &_get, &_set, NULL, NULL, NULL);
    iupClassRegisterAttribute(ic, "EXPAND", _get, _set, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  }

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* IupCanvas only */
  iupClassRegisterAttribute(ic, "CURSOR", NULL, iupdrvBaseSetCursorAttrib, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "XMIN", NULL, NULL, IUPAF_SAMEASSYSTEM, "0.0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "XMAX", NULL, NULL, IUPAF_SAMEASSYSTEM, "1.0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "YMIN", NULL, NULL, IUPAF_SAMEASSYSTEM, "0.0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "YMAX", NULL, NULL, IUPAF_SAMEASSYSTEM, "1.0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEX", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LINEY", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BORDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "SCROLLBAR", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupdrvCanvasInitClass(ic);

  return ic;
}
