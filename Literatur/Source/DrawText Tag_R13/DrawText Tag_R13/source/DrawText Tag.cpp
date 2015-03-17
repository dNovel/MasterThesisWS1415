                          // This example draws text as a bitmap image with an alpha background to the screen like a HUD display
                          // Thanks to Matthias Bober who provided most of the code I needed to create this example plugin

#include "c4d.h"
#include "c4d_symbols.h"
#include "tsimpletag.h"
#include "customgui_priority.h"  //needed for the priority stuff to work
#include "..\..\..\..\resource\_api\c4d_libs\lib_clipmap.h"


#define PLUGIN_ID    1029034


class SimpleTag : public TagData
{
    INSTANCEOF(SimpleTag,TagData)

    public:
    virtual Bool Message(GeListNode *node, LONG type, void *t_data);
    virtual Bool Init(GeListNode *node);
    virtual Bool GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags);	
    virtual Bool GetDParameter(GeListNode *node, const DescID &id,GeData &t_data,DESCFLAGS_GET &flags);
    virtual Bool SetDParameter(GeListNode *node, const DescID &id,const GeData &t_data,DESCFLAGS_SET &flags);
    virtual Bool Draw(BaseTag* tag, BaseObject* op, BaseDraw* bd, BaseDrawHelp* bh);

    virtual EXECUTIONRESULT Execute(BaseTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, EXECUTIONFLAGS flags);
		
    static NodeData *Alloc(void) { return gNew SimpleTag; }

	private:
	Real PolygonsHUDX;
	Real PolygonsHUDY;   //Some class member variables that we can use in any of the methods without scope limitations
	Real PointsHUDX;
	Real PointsHUDY;

};


Bool SimpleTag::GetDParameter(GeListNode *node, const DescID &id, GeData &t_data, DESCFLAGS_GET &flags) //Used to get the decriptions data 
{ 
    return SUPER::GetDParameter(node, id, t_data, flags);
}

Bool SimpleTag::SetDParameter(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_SET &flags) //Used to change the decriptions data
{ 
    return SUPER::SetDParameter(node, id, t_data, flags);
}

Bool SimpleTag::GetDDescription(GeListNode *node, Description *description,DESCFLAGS_DESC &flags)
{ 
    //flags |= DESCFLAGS_DESC_LOADED;
    return TRUE;
}



Bool SimpleTag::Message(GeListNode *node, LONG type, void *data)
{

    BaseTag	*tag = (BaseTag*)node;                        //Get the tag and assign it to a variable 

    //Do something
 
    tag->SetDirty(DIRTYFLAGS_DATA); //Used to update a Tag's AM GUI items

    return TRUE;
}

Bool SimpleTag::Init(GeListNode *node)
{   // intitial values for the tag when it's created are set in this section

    BaseTag	*tag  = (BaseTag*)node;           //Assigns a variable to the tag's node
    BaseContainer *data = tag->GetDataInstance(); //Assigns a variable to that node's container
    data->SetBool(MYBOX,FALSE);          //Sets the checkbox to disabled by default when tag is created-->looks in the description->tbasictag.h file for matching name
    data->SetReal(POLYHUDX,0);
	data->SetReal(POLYHUDY,255);

    data->SetReal(POINTHUDX,5);
	data->SetReal(POINTHUDY,270);
 

    return TRUE;
}

Bool SimpleTag::Draw(BaseTag *tag, BaseObject *op, BaseDraw *bd, BaseDrawHelp *bh )
{
    //bd = doc->GetActiveBaseDraw();           //This is not needed because BaseDraw is already created in the parenths
    //if(bd) return FALSE; 

	//Lets get the object's stats...Polygons, points, and how many of each
	BaseObject *obj = tag->GetObject();
	PointObject *pntObj = ToPoint(obj);
	Vector *allpoints = pntObj->GetPointW();
	LONG pntCount = pntObj->GetPointCount();
	PolygonObject *polyObj = ToPoly(obj);
	CPolygon *polys = ToPoly(polyObj)->GetPolygonW(); 
	LONG plyCount = polyObj->GetPolygonCount();


    if(tag->GetData().GetBool(MYBOX))          //If the 'MYBOX' checkbox attribute is enabled
    { 	

    //There is an OpenGL problem with most video cards that makes the drawn object get covered up by objects in the scene
    //This is a work around to deal with that problem
    //We need to draw a dummy 2D object first...Then draw the actual object we want to see as a 2D HUD object
        bd->SetMatrix_Screen();                             //Use the screen's matrix to draw on
        bd->DrawLine2D(Vector(0, 0, 0), Vector(0, 0, 0));   //Draw a line with a zero length<---This is our dummy object
        bd->SetDepth(TRUE);                                 //This fixes drawing problems when using 2D functions


        //Now we can create the text HUD object that we want to see in the view	

        AutoAlloc<GeClipMap> cm;              //Create an instance of the GeClipMap class
        if(!cm) return FALSE;
		        
		String polycountToString = FormatNumber(plyCount, FORMAT_LONG, NULL, TRUE);   //Converts Long type to a string type
		String plys = "PolyCount: ";
		String objPolygonCount = plys.operator+=(polycountToString);	

        cm->Init(0, 0, 32);                   //Initializes the GeClipMap class to use this variable(cm) as 32 bit
        cm->BeginDraw();                      //Start drawing the object. This must be used before drawing the text
        LONG width = cm->GetTextWidth(objPolygonCount);  //Gets the width of the text
        LONG height = cm->GetTextHeight();    //Gets the width of the text
        cm->EndDraw();                        //Tell C4D we're done drawing the text
        cm->Destroy();                        //Release any memory used by the GeClipMap class

        cm->Init(width, height, 32);          //Dynamically sets the size of the virtual plane the text is on
        cm->BeginDraw();
        cm->SetColor(255, 255, 255, 255);     //Sets the color of the text to white
        cm->TextAt(0,0,objPolygonCount);
        cm->EndDraw();

        bd->SetMatrix_Screen();                        //We always need a matrix to draw things...In this case we'll use the screen's matrix
        bd->SetLightList(BDRAW_SETLIGHTLIST_NOLIGHTS); //Use other options for different results if desired

        //Now we create the four vector variables that will determine the Size&Shape of the text we're drawing
        //NOTE: We're creating a four point plane that has a text image on it...Not text created with a bunch of vectors
        Vector *padr = bNew Vector[4];
        Vector *cadr = bNew Vector[4];
        Vector *vnadr = bNew Vector[4];
        Vector *uvadr = bNew Vector[4];

        LONG xpos=PolygonsHUDX;                    //The X screen location of the left upper corner of the plane object the text bitmap is on
        LONG ypos=PolygonsHUDY;                    //The Y screen location of the left upper corner of the plane object the text bitmap is on


        //Now we set the actual vector postions for the four point plane object that holds the text bitmap
        padr[0] = Vector(xpos,ypos,0);                      //upper left corner
        padr[1] = Vector(xpos+width,ypos,0);                //upper right corner
        padr[2] = Vector(xpos+width,ypos+height,0);         //lower right corner
        padr[3] = Vector(xpos,ypos+height,0);               //lower left corner

        //Now we set the color vector values for the plane object 
		cadr[0] = Vector(1,1,1);
        cadr[1] = Vector(1,1,1);     //The text will be white
        cadr[2] = Vector(1,1,1);
        cadr[3] = Vector(1,1,1);

       //Now we set up the normals directions for the four point plane object that holds the text bitmap
        vnadr[0] = Vector(0,0,1);
        vnadr[1] = Vector(0,0,1);
        vnadr[2] = Vector(0,0,1);
        vnadr[3] = Vector(0,0,1);

       //Now we set up the UV's for the four point plane object that holds the text bitmap
        uvadr[0] = Vector(0,0,0);
        uvadr[1] = Vector(1,0,0);
        uvadr[2] = Vector(1,1,0);
        uvadr[3] = Vector(0,1,0);

        BaseBitmap *cmbmp = NULL;
        cmbmp = cm->GetBitmap();                         //Get the bitmap we're using and assign it to a variable(cmbmp)
        if(!cmbmp) return FALSE;

        BaseBitmap *bmp = NULL;
        bmp = cmbmp->GetClone();                         //Get a copy of that bitmap variable so we can create an alpha version of it
        if(!bmp) return FALSE;

        BaseBitmap *alpha = NULL;
        alpha = bmp->GetInternalChannel();               //Get at the RGBA channels of the bitmap copy
        if(!alpha) alpha = bmp->AddChannel(TRUE, FALSE); //Makes the copy an alpha type of bitmap
        if(!alpha)
        {
          BaseBitmap::Free(bmp);
          return FALSE;
        }

        //Apply the alpha bitmap to the solution so only the text is visible on the screen
		//The alpha acts like a matte object. So if you find that your text is getting cut off..Your alpha might be too small
		//You can increase the width and height of the alpha to fix that problem
		LONG x,y;
        for(y=0; y<height; y++)
        {
          for(x=0; x<width; x++)
          {
             UWORD r;                                //Create an unsigned short int variable...Can hold absolute values between 0->65,535
             bmp->GetPixel(x,y,&r,&r,&r);            //Get each pixel's X.Y coords and assign them to pointers (r,g,b) (0 <= r/g/b <= 255) 
             bmp->SetAlphaPixel(alpha, x, y, r);     //r is the opacity
          }
        }

        bd->DrawTexture(bmp,padr,cadr,vnadr,uvadr,4,DRAW_ALPHA_NORMAL,DRAW_TEXTUREFLAGS_0); //Does that actual drawing of the bitmap to the screen

        BaseBitmap::Free(bmp);    //Free any memory used by BaseBitmap
        bDelete(padr);
        bDelete(cadr);            //Delete all the empty pointers
        bDelete(vnadr);
        bDelete(uvadr);

       //2D drawing functions disable the Z buffer making object's in the scene half invisible
       //There are two ways to fix this:
       bd->SetDepth(TRUE);                         //This fixes the OpenGL problem
       //bd->SetMatrix_Matrix(op, bh->GetMg(),5);  //This also fixes the OpenGL problem



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	   /////////////////////// Lets create a second text HUD obj////////////////////////////////////////////



        //Now we can create the text HUD object that we want to see in the view	

        AutoAlloc<GeClipMap> cm2;                //Create an instance of the GeClipMap class
        if(!cm2) return FALSE;

        //String text2 = "I'm Bigger";             //The text that we will see in the HUD
		String countToString = FormatNumber(pntCount, FORMAT_LONG, NULL, TRUE);   //Converts Long type to a string type
		String pnts = "PointCount: ";
		String objPointCount = pnts.operator+=(countToString);		
		
        cm2->Init(0, 0, 32);                     //Initializes the GeClipMap class to use this variable(cm) as 32 bit
        cm2->BeginDraw();                        //Start drawing the object. This must be used before drawing the text
        LONG width2 = cm2->GetTextWidth(objPointCount);  //Gets the width of the text
        LONG height2 = cm2->GetTextHeight();     //Gets the height of the text
        cm2->EndDraw();                          //Tell C4D we're done drawing the text
        cm2->Destroy();                          //Release any memory used by the GeClipMap class
				
        cm2->Init(width2, height2, 32);          //Dynamically sets the size of the virtual plane the text is on
        cm2->BeginDraw();
        cm2->SetColor(255, 255, 255, 255);       //Sets the color of the text to white
        cm2->TextAt(0,0,objPointCount);          //Positions the text within the virtual plane object that's holding it
        cm2->EndDraw();

        bd->SetMatrix_Screen();                        //We always need a matrix to draw things...In this case we'll use the screen's matrix
        bd->SetLightList(BDRAW_SETLIGHTLIST_NOLIGHTS); //Use other options for different results if desired

        //Now we create the four vector variables that will determine the Size&Shape of the text we're drawing
        //NOTE: We're creating a four point plane that has a text image on it...Not text created with a bunch of vectors
        Vector *padr2 = bNew Vector[4];
        Vector *cadr2 = bNew Vector[4];
        Vector *vnadr2 = bNew Vector[4];
        Vector *uvadr2 = bNew Vector[4];

        LONG xpos2=PointsHUDX;                    //The X screen location of the left upper corner of the plane object the text bitmap is on
        LONG ypos2=PointsHUDY;                    //The Y screen location of the left upper corner of the plane object the text bitmap is on


        //Now we set the actual vector postions for the four point plane object that holds the text bitmap
		//Note how I added 5 to all the corners..Thus scaling up the text
        padr2[0] = Vector(xpos2-5, ypos2-5, 0);                   //upper left corner
        padr2[1] = Vector(xpos2+width2+5, ypos2-5, 0);            //upper right corner
        padr2[2] = Vector(xpos2+width2+5, ypos2+height2+5, 0);    //lower right corner
        padr2[3] = Vector(xpos2-5, ypos2+height2+5, 0);           //lower left corner


        //Now we set the color vector values for the plane object 
		cadr2[0] = Vector(1,0,0);   //We can set each corner to it's own color value to create gradient type effects
        cadr2[1] = Vector(1,0,0);
        cadr2[2] = Vector(1,1,1);
        cadr2[3] = Vector(1,1,1);

       //Now we set up the normals directions for the four point plane object that holds the text bitmap
        vnadr2[0] = Vector(0,0,1);
        vnadr2[1] = Vector(0,0,1);
        vnadr2[2] = Vector(0,0,1);
        vnadr2[3] = Vector(0,0,1);

       //Now we set up the UV's for the four point plane object that holds the text bitmap
        uvadr2[0] = Vector(0,0,0);
        uvadr2[1] = Vector(1,0,0);
        uvadr2[2] = Vector(1,1,0);
        uvadr2[3] = Vector(0,1,0);

        BaseBitmap *cmbmp2 = NULL;
        cmbmp2 = cm2->GetBitmap();                         //Get the bitmap we're using and assign it to a variable(cmbmp)
        if(!cmbmp2) return FALSE;

        BaseBitmap *bmp2 = NULL;
        bmp2 = cmbmp2->GetClone();                         //Get a copy of that bitmap variable so we can create an alpha version of it
        if(!bmp2) return FALSE;

        BaseBitmap *alpha2 = NULL;
        alpha2 = bmp2->GetInternalChannel();               //Get at the RGBA channels of the bitmap copy
        if(!alpha2) alpha2 = bmp2->AddChannel(TRUE, FALSE); //Makes the copy an alpha type of bitmap
        if(!alpha2)
        {
          BaseBitmap::Free(bmp2);
          return FALSE;
        }

        //Apply the alpha bitmap to the solution so only the text is visible on the screen
		//The alpha acts like a matte object. So if you find that your text is getting cut off..Your alpha might be too small
		//You can increase the width and height of the alpha to fix that problem
		LONG x2,y2;
        for(y2=0; y2<height+10; y2++)
        {
          for(x2=0; x2<width+10; x2++)                   //increasing the width value makes increases the alpha's size
          {
             UWORD r2;
             bmp2->GetPixel(x2,y2,&r2,&r2,&r2);
             bmp2->SetAlphaPixel(alpha2, x2, y2, r2);     //r2 is the opacity
          }
        }

        bd->DrawTexture(bmp2,padr2,cadr2,vnadr2,uvadr2,4,DRAW_ALPHA_NORMAL,DRAW_TEXTUREFLAGS_0); //Does that actual drawing of the bitmap to the screen

        BaseBitmap::Free(bmp2);    //Free any memory used by BaseBitmap
        bDelete(padr2);
        bDelete(cadr2);            //Delete all the empty pointers
        bDelete(vnadr2);
        bDelete(uvadr2);

       //2D drawing functions disable the Z buffer making object's in the scene half invisible
       //There are two ways to fix this:
       bd->SetDepth(TRUE);                         //This fixes the OpenGL problem
       //bd->SetMatrix_Matrix(op, bh->GetMg(),5);  //This also fixes the OpenGL problem


    }	

    return TRUE;
}


EXECUTIONRESULT SimpleTag::Execute(BaseTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, EXECUTIONFLAGS flags)
{

    //BaseContainer *data = tag->GetDataInstance();    //Assigns a variable to that node's container

    Real value1 = tag->GetData().GetReal(POLYHUDX);    //Get the value of the gizmo
    PolygonsHUDX = value1;                             //Pass the value to the class member variable that positions the custom HUD element

    Real value2 = tag->GetData().GetReal(POLYHUDY);    //Get the value of the gizmo
    PolygonsHUDY = value2;                             //Pass the value to the class member variable that positions the custom HUD element

    Real value3 = tag->GetData().GetReal(POINTHUDX);  //Get the value of the gizmo
    PointsHUDX = value3;                              //Pass the value to the class member variable that positions the custom HUD element

    Real value4 = tag->GetData().GetReal(POINTHUDY);  //Get the value of the gizmo
    PointsHUDY = value4;                              //Pass the value to the class member variable that positions the custom HUD element

    return EXECUTIONRESULT_OK;
}

Bool RegisterSimpleTag(void)
{
    String path=GeLoadString(IDS_SIMPLETAG); if (!path.Content()) return TRUE; // points to the res->c4d_symbols file which conatins the enum "IDS_SIMPLETAG" entry 
    return RegisterTagPlugin(PLUGIN_ID,path,TAG_EXPRESSION|TAG_VISIBLE,SimpleTag::Alloc,"tsimpletag",AutoBitmap("myicon.png"),0);
}