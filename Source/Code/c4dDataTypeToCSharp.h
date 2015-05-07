#include "c4d_tagdata.h"
#include "c4d_tagplugin.h"

// "c4d_tagdata.h"
//%include "c4d_tagdata.h";
%feature("director") TagDataM;
%csmethodmodifiers TagDataM::TagDataM "private";
%typemap(cscode) TagDataM %{
	public TagDataM(bool memOwn) : this(C4dApiPINVOKE.new_TagDataM(), memOwn) {
    SwigDirectorConnect();
  }
%}
%include "c4d_tagdata.h";
%include "TagDataM.h";

// "c4d_tagplugin.h"
//%include "c4d_tagplugin.h";
%feature("director") TagPlugin;
%csmethodmodifiers TagPlugin::TagPlugin "private";
%typemap(cscode) TagPlugin %{
  public TagPlugin(bool memOwn) : this(C4dApiPINVOKE.new_TagPlugin(), memOwn) {
    SwigDirectorConnect();
  }
%}
%include "c4d_tagplugin.h";