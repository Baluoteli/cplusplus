#ifndef __MACDEF_H__
#define __MACDEF_H__ 

/* Contain variety of Define for cPlusPlus
*/

namespace plusFCL_BTL
{
	/*Export class Instance for static or Dynamic
	*/
#define __DECLAREEXPORTCLASS(className)\
className* get##className##Instance();

#define __IMPLEMENTEXPORTCLASS(className)\
className* get##className##Instance() {\
return new className; }

#define __DECLAREEXPORTCLASSMUTIL(className,Instance)\
className* get##className##Instance(Instance);

#define __IMPLEMENTEXPORTCLASSMUTIL(className,Instance)\
className* get##className##Instance(Instance) {\
return new className(Instance); }

#define MAXPATHLEN 128


  /* @Detail: InI File
  */
	
  /* @Detail Xml File
  */

  /* @Detail JSON File
  */
  
  /* @Block Mgr
  */
}

#endif