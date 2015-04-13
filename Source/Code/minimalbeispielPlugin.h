#define ID_PLUGINID 1234567 // Dies ist eine einzigartige Plugin ID.
 
#include "c4d.h"
 
class MinimalesPlugin : public CommandData // Das Plugin ist vom Plugin Typ "Command Plugin"
{
public:
  virtual Bool Execute(BaseDocument *doc)
  {    
    GePrint("Eine Ausgabe zum Cinema 4D Konsolenfenster.");
    return TRUE;
  }
};
 
// Mit dieser funktion steigt das Plugin ein.
Bool PluginStart(void)
{
  return RegisterCommandPlugin(ID_PLUGINID,"MinimalesPlugin", 0,NULL,String("MinimalesPlugin"), gNew MinimalesPlugin);
}
 
 // Wird beim beenden des Plugin aufgerufen.
void PluginEnd(void)
{
}

// Ist teil des Message Systems von C4d. Dient der Kommunikation. 
Bool PluginMessage(LONG id, void *data)
{
  return TRUE;
}