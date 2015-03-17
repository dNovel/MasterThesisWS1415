/////////////////////////////////////////////////////////////
// A Tag to snap an object to spline foot steps based on frame times
// Thanks to Robert Templeton for helping me with the dynamic GUI stuff


#include "c4d.h"
#include "c4d_symbols.h"
#include "tquicksteps.h"
#include "customgui_priority.h"  //Needed for the priority stuff to work
#include "ge_dynamicarray.h"     //Needed if any C4D dynamic arrays are used in the code

#define PLUGIN_ID	1028524 

Bool BakeKeys(BaseObject *myObject, Real nthframe)
{
    BaseDocument *mydoc = GetActiveDocument();
	BaseObject *myobj = myObject;
    BaseTime time = mydoc->GetTime();
    Real fps = mydoc->GetFps();
    Real userStartTime = mydoc->GetMinTime().GetFrame(fps);      //The Min range slider
    Real userEndTime = mydoc->GetMaxTime().GetFrame(fps);        //The Max range slider

    Real nframe = nthframe;                              //Bakes every nthframe. Set as needed
	Real frame = mydoc->GetTime().GetFrame(fps); 

    mydoc->SetTime(BaseTime(userStartTime, fps));           //set the time slider to the user's start time	
    Real currentFrame = mydoc->GetTime().GetFrame(fps);     //Get the frame the slider is currently on

    while(currentFrame <= userEndTime)
     {
       StopAllThreads();
	   StatusSetSpin();
	   StatusSetText("Baking in Progress...Please Wait");
       CallCommand(12410);                           //Record a keyframe
       userStartTime = userStartTime + nthframe;     //Bakes every nthframe
	   mydoc->SetTime(BaseTime(userStartTime, fps)); 
       Bool redraw = DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_REDUCTION|DRAWFLAGS_STATICBREAK); //Allows C4D to update the changes going on
       EventAdd(EVENT_ANIMATE); // Move the timeline scrubber
       if(redraw == TRUE) currentFrame = mydoc->GetTime().GetFrame(fps);
  }
	StatusClear();
	mydoc->SetTime(BaseTime(0, fps));    //Set the scrubber back to zero

	return TRUE;
}


Bool DeletePSRTracks(BaseObject *delObject)
{
	BaseObject *dobj = delObject;
	CTrack *ft = dobj->GetFirstCTrack();

	CTrack *Xpostrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_POSITION,DTYPE_VECTOR,0), DescLevel(VECTOR_X,DTYPE_REAL,0)));
    if(Xpostrack) Xpostrack->Remove();
	CTrack *Ypostrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_POSITION,DTYPE_VECTOR,0), DescLevel(VECTOR_Y,DTYPE_REAL,0)));
    if(Ypostrack) Ypostrack->Remove();
	CTrack *Zpostrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_POSITION,DTYPE_VECTOR,0), DescLevel(VECTOR_Z,DTYPE_REAL,0)));
    if(Zpostrack) Zpostrack->Remove();

	CTrack *Xscaletrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_SCALE,DTYPE_VECTOR,0), DescLevel(VECTOR_X,DTYPE_REAL,0)));
    if(Xscaletrack) Xscaletrack->Remove();
	CTrack *Yscaletrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_SCALE,DTYPE_VECTOR,0), DescLevel(VECTOR_Y,DTYPE_REAL,0)));
    if(Yscaletrack) Yscaletrack->Remove();
	CTrack *Zscaletrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_SCALE,DTYPE_VECTOR,0), DescLevel(VECTOR_Z,DTYPE_REAL,0)));
    if(Zscaletrack) Zscaletrack->Remove();

	CTrack *Xrottrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_ROTATION,DTYPE_VECTOR,0), DescLevel(VECTOR_X,DTYPE_REAL,0)));
    if(Xrottrack) Xrottrack->Remove();
	CTrack *Yrottrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_ROTATION,DTYPE_VECTOR,0), DescLevel(VECTOR_Y,DTYPE_REAL,0)));
    if(Yrottrack) Yrottrack->Remove();
	CTrack *Zrottrack = dobj->FindCTrack(DescID(DescLevel(ID_BASEOBJECT_REL_ROTATION,DTYPE_VECTOR,0), DescLevel(VECTOR_Z,DTYPE_REAL,0)));
    if(Zrottrack) Zrottrack->Remove(); 
  
	return TRUE;
}

class QuickSteps : public TagData
{
	public:
		virtual Bool Message(GeListNode *node, LONG type, void *t_data);
		virtual Bool Init(GeListNode *node);
		virtual Bool GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags);		
		virtual EXECUTIONRESULT Execute(BaseTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, EXECUTIONFLAGS flags);
				
		static NodeData *Alloc(void) { return gNew QuickSteps; }
};

Bool QuickSteps::Message(GeListNode *node, LONG type, void *data)
{
    BaseTag	*tag  = (BaseTag*)node;	
	BaseContainer *tagdata = tag->GetDataInstance();


	///// This section sets up the scene to bake keyframes when the a button is pressed /////////
    Real keyInterp = tag->GetData().GetReal(NUMofKEYS);//Grabs the GUI gizmo with the NUMofKEYS ID
	Real frame = tag->GetData().GetReal(NUMofKEYS);//Grabs the GUI gizmo with the NUMofKEYS ID
    BaseDocument *doc = GetActiveDocument();
	BaseObject *obj = tag->GetObject();

	BaseTime time = doc->GetTime();
    Real fps = doc->GetFps();
	Real getFrame = time.GetFrame(fps);

    Real count = tagdata->GetReal(NUMofGUIS); //The total number of steps created
	GeDynamicArray<Real>myarray(0);           //Create a dynamic array with one element to start off with

	switch (type)
    {        
        case MSG_DESCRIPTION_COMMAND:                               //MSG_DESCRIPTION_COMMAND is send when button is clicked
        {            
            DescriptionCommand *dc = (DescriptionCommand*)data;    //Data contains the description ID's of the items in the tag
            LONG button = dc->id[0].id;                            //Gets the ID of BUTTONS based on the quicksteps.h enum values and assigns it to this variable
			Bool checkbox = dc->id[0].id;			               //Gets the ID of CHECKBOXES based on the quicksteps.h enum values and assigns it to this variable
            
            // check for different button IDs
            switch (button)
            {

                 case BTN_BAKE_STEPS:
					   CallCommand(100004767);      //Deselect All objects(we only want to create keys for the object the tag is on)
					   obj->SetBit(BIT_ACTIVE);     //Select the object that's holding the tag
					   DeletePSRTracks(obj);        //Delete any previous PSR tracks so we start fresh every time this button is pressed 
	                   for(int i=0; i<count; i++)
	                   {
		                Real frameValue = tagdata->GetReal(4000+i); //Get the frame value of each step
						myarray.Push(frameValue);                   //Add each step's frame value to the array					
						doc->SetTime(BaseTime(getFrame + myarray[i], fps));	
						//GePrint(RealToString(myarray[i]));
						Bool redraw = DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_REDUCTION|DRAWFLAGS_STATICBREAK); //Allows C4D to update the changes going on
						
						if(tagdata->GetObjectLink(5000+i, doc))      //If there is an object in the linkfield
						 {						
						  CallCommand(12410);                         //Record a keyframe	
						 }
                        EventAdd(EVENT_ANIMATE); // Move the timeline scrubber						
	                   }
					   doc->SetTime(BaseTime(0, fps));  //Reset the slider back to zero					   
					   tag->SetBit(BIT_ACTIVE);        //Re-select the tag
                    break;


              case BTN_BAKE_KEYS:
					obj->SetBit(BIT_ACTIVE);                           //Select the object to bake keys on(the tag's object)					
					if(!keyInterp)BakeKeys(obj, 1); // WARNING!!!...The NUMofKEYS GUI is not created until the user changes it..So we need to have a defualt value                      
					else BakeKeys(obj, keyInterp);                     //Execute the bake method using the "NUMofKEYS" GUI as the control					
					obj->DelBit(BIT_ACTIVE);                           //De-Select the object
                    break;


              case BTN_DELETE_KEYS:
					DeletePSRTracks(obj);        //Calls to the custom method we made to delete the PSR tacks
                    break; 
            }
        }
    }               

	myarray.~GeDynamicArray();   //Destructor to free any memory used by the array
    return TRUE;
}

Bool QuickSteps::Init(GeListNode *node)  // intitial values for the tag when it's created are set in this section 
{	
	BaseTag	*tag  = (BaseTag*)node; // Assigns a variable to the tag's node
	BaseContainer *data = tag->GetDataInstance(); // Assigns a variable to that node's container	
	    
	//This code sets up the expression priority options that most things have in c4d 
	GeData d;
	if (node->GetParameter(DescLevel(EXPRESSION_PRIORITY),d,DESCFLAGS_GET_0))
	{
		PriorityData *pd = (PriorityData*)d.GetCustomDataType(CUSTOMGUI_PRIORITY_DATA);
		if (pd) pd->SetPriorityValue(PRIORITYVALUE_CAMERADEPENDENT,GeData(FALSE));
		node->SetParameter(DescLevel(EXPRESSION_PRIORITY),d,DESCFLAGS_SET_0);
	}


	return TRUE;
}


Bool QuickSteps::GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags)
{
    BaseContainer *data;       //Create an empty container
    const DescID *singleid;    //Create a DescID variable

    if(!description->LoadDescription(node->GetType()))
        return FALSE;

    data = ((BaseList2D*)node)->GetDataInstance();//Get the container for the tag
    singleid = description->GetSingleDescID();    // <---- undocumented function call


  ///////////  This section creates multiple LINKBOX's & NumEdit boxes based on the NUMofGUIS EDITTEXT GUI gizmo value /////////////

    Bool myswitch = FALSE;                        //A switching variable to be used later on 
	BaseContainer frameBc;                        //The container variable for all of the NumEdit fields we'll create later
    BaseContainer linkboxBc;                      //The container variable for all of the linkbox's we'll create later

	DescID cid = DescLevel(6001, DTYPE_GROUP, 0);  //This is the maingroup's ID

	if(!singleid || cid.IsPartOf(*singleid,NULL))  //important to check for speedup c4d!
	{
	    BaseContainer maingroupBc = GetCustomDataTypeDefault(DTYPE_GROUP);
	    maingroupBc.SetString(DESC_NAME, "STEPS");  //The name of the group(AM tab)
	    maingroupBc.SetLong(DESC_COLUMNS, 2);
	    maingroupBc.SetReal(DESC_DEFAULT, 1);        //Makes this group(AM tab) selected by default
	    if(!description->SetParameter(cid, maingroupBc, DescLevel(0))) return TRUE; //DescLevel() is zero because this is the main group
	}


    //Create the two GUI's and assign unique ID numbers for the NumEdit's vs the LinkBox's so they can be accessed individually if needed
    LONG index = data->GetReal(NUMofGUIS)+4000L; //Get the numeric value from this GUI item to determine how many GUI's to create..Plus assign an ID number to them
	for (int i=4000L; i<index; i++)
    {
    if (!myswitch)
     {
      myswitch = TRUE;
	  cid = DescLevel(i, DTYPE_REAL, 0);
	  if(!singleid || cid.IsPartOf(*singleid,NULL)) // important to check for speedup c4d!
	  {
		frameBc = GetCustomDataTypeDefault(DTYPE_REAL); //IMPORTANT!!! values will change to zero without using this !!!
	    frameBc.SetLong(DESC_CUSTOMGUI,CUSTOMGUI_REAL);
	    frameBc.SetBool(DESC_FORBID_SCALING,TRUE);
	    frameBc.SetBool(DESC_FORBID_INLINE_FOLDING, FALSE);
		frameBc.SetString(DESC_NAME,"Frame#");
		if(!description->SetParameter(cid, frameBc, DescLevel(6001))) return TRUE; //The DescLevel(id) is the maingroup's id so it's placed inside of that group

		LONG index2 = 1000;                                                  //We need to give the linkbox's their own unique ID number values
		cid = DescLevel(i+index2, DTYPE_LONG, 0);                            //So we bump the previous NumEdit values up by a factor of 1000
		frameBc = GetCustomDataTypeDefault(DTYPE_LONG); //IMPORTANT!!! values will change to zero without using this !!!
		linkboxBc.SetLong(DESC_CUSTOMGUI,CUSTOMGUI_LINKBOX);
        linkboxBc.SetBool(DESC_SCALEH,TRUE);
		linkboxBc.SetString(DESC_NAME,"Controller Object_" + LongToString((i+index2)-4999));
		index2++;
	    if(!description->SetParameter(cid, linkboxBc, DescLevel(6001))) return TRUE; //The DescLevel(id) is the maingroup's id so it's placed inside of that group
	  }
     }
      myswitch = FALSE;  //Reset the variable back to false
    }



    flags |= DESCFLAGS_DESC_LOADED;
    return TRUE;
}


EXECUTIONRESULT QuickSteps::Execute(BaseTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, EXECUTIONFLAGS flags)
{
	BaseContainer *tagdata = tag->GetDataInstance();  //Get the data in the tag and assign it to a variable
	BaseTime time = doc->GetTime();                   //Assigns the basetime class to a variable
	Real fps = doc->GetFps();                         //Gets the document's FPS
    Real frame = time.GetFrame(fps);                  //The current frame the slider is on


 	////////// Process the gui's ////////////
 	Real count = tagdata->GetReal(NUMofGUIS);
	for(int i=0; i<count; i++)
	{
		Real frameValue = tagdata->GetReal(4000+i);
		BaseObject *FootstepObject = tagdata->GetObjectLink(5000+i, doc);

		if(frame == frameValue && FootstepObject != NULL)
		{
	      Matrix m1 = FootstepObject->GetMg();      //Get the object's global matrix
	      op->SetMg(m1);                            //Set the object to the step object's global matrix(pos,scale,rot)
		  op->Message(MSG_UPDATE);
		}
	}
	
	EventAdd();

	return EXECUTIONRESULT_OK;
}

Bool RegisterQuickSteps(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String path=GeLoadString(IDS_QUICKSTEPS); if (!path.Content()) return TRUE; // points to the res->c4d_symbols file which conatins the enum "IDS_SIMPLETAG" entry 
	return RegisterTagPlugin(PLUGIN_ID,path,TAG_EXPRESSION|TAG_VISIBLE,QuickSteps::Alloc,"tquicksteps",AutoBitmap("myicon.PNG"),0);
}





////////////////////// This is how to write the same code if you don't use a tag and want to search the OM for the steps /////////////////////


//EXECUTIONRESULT QuickSteps::Execute(BaseTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, EXECUTIONFLAGS flags)
//{
	//if(doc->SearchObject("Steps") == NULL) return EXECUTIONRESULT_OK;
	//BaseObject *countsteps = doc->SearchObject("Steps")->GetDown(); //Get the first child under the parent object called "Steps"
	//if(!countsteps) return EXECUTIONRESULT_OK;
	//
	//int stepcount = 0;	
 //   while(countsteps)
 //     {
 //      stepcount ++;                            //Counts the number of child objects(steps) in the group	        
 //      countsteps = countsteps->GetNext();
 //     }

 //   GeDynamicArray<BaseObject *> steparr(stepcount);  // Dynamic array using the value from stepcount to set it's size dynamically
 //   BaseObject* nextObj = doc->SearchObject("Steps")->GetDown(); //Get the first child under the parent object called "Steps"
 //   for(int i = 0; i<stepcount;i++)
 //    {
 //    steparr[i] = nextObj;                 //Fill the steparr array with the objects found during each loop iteration
 //    nextObj = nextObj->GetNext();
 //   }


	//GePrint(steparr[0]->GetName());
	//GePrint(steparr[1]->GetName());
	//GePrint(steparr[2]->GetName());

  // if(frame == 0)
  //  {	 
	 //Matrix footmg = steparr[0]->GetMg();        //Get the global matrix of the first step in the stepsarr array
  //   op->SetMg(footmg);                          //Set the object the tag is on to the same position and rotation
  // } 

  // if(frame == 10)
  //  {	 
	 //Matrix footmg = steparr[1]->GetMg();        //Get the global matrix of the first step in the stepsarr array
  //   op->SetMg(footmg);                          //Set the object the tag is on to the same position and rotation
  //  }

  // if(frame == 20)
  //  {
	 //Matrix footmg = steparr[2]->GetMg();        //Get the global matrix of the first step in the stepsarr array
  //   op->SetMg(footmg);                          //Set the object the tag is on to the same position and rotation
  //  }
	//return EXECUTIONRESULT_OK;
//}