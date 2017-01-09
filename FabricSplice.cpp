// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "LoggingImpl.h"
#include "SceneManagementImpl.h"
#include "DGGraphImpl.h"
#include "KLParserImpl.h"
#include "FabricSplice.h"

#define quoted(s) #s
#define coreversionconcat(a, b) quoted(a) "." quoted(b)
#define spliceversionconcat(a, b, c) quoted(a) "." quoted(b) "." quoted(c)

using namespace FabricSpliceImpl;

bool gInitialized = false;

#define FECS_TRY try {
#define FECS_TRY_CLEARERROR try { LoggingImpl::clearError();
#define FECS_CATCH_VOID } catch(FabricCore::Exception e) { LoggingImpl::reportError(e.getDesc_cstr()); return; }
#define FECS_CATCH(value) } catch(FabricCore::Exception e) { LoggingImpl::reportError(e.getDesc_cstr()); return value; }

#define GETSMARTPTRVOID(ptrType, element) \
  ptrType * ptr = (ptrType *)ref; \
  if(ptr == NULL) \
    return; \
  ptrType & element = *ptr; \
  if(!element) \
    return;

#define GETSMARTPTR(ptrType, element, returnVal) \
  ptrType * ptr = (ptrType *)ref; \
  if(ptr == NULL) \
    return returnVal; \
  ptrType & element = *ptr; \
  if(!element) \
    return returnVal;

#define GETPTRVOID(ptrType, element) \
  ptrType * element = (ptrType *)ref; \
  if(element == NULL) \
    return;

#define GETPTR(ptrType, element, returnVal) \
  ptrType * element = (ptrType *)ref; \
  if(element == NULL) \
    return returnVal;

void FECS_Initialize()
{
  FECS_TRY_CLEARERROR
  if(gInitialized)
    return;
  FabricCore::Initialize();
  LoggingImpl::log("Initialized FabricSplice.");
  gInitialized = true;
  FECS_CATCH_VOID
}

void FECS_Finalize()
{
  FECS_TRY_CLEARERROR
  if(!gInitialized)
    return;
  FabricCore::Finalize();
  LoggingImpl::log("Finalized FabricSplice.");
  gInitialized = false;
  FECS_CATCH_VOID
}

uint8_t FECS_GetFabricVersionMaj()
{
  return FabricCore::GetVersionMaj();
}

uint8_t FECS_GetFabricVersionMin()
{
  return FabricCore::GetVersionMin();
}

uint8_t FECS_GetFabricVersionRev()
{
  return FabricCore::GetVersionRev();
}

const char * FECS_GetFabricVersionStr()
{
  return FabricCore::GetVersionStr();
}

const char * FECS_GetSpliceVersion()
{
  return spliceversionconcat(SPLICE_MAJOR_VERSION, SPLICE_MINOR_VERSION, SPLICE_REVISION_VERSION);
}

void FECS_constructClient(FabricCore::Client & client, int guarded, FabricCore::ClientOptimizationType optType)
{
  FECS_TRY_CLEARERROR
  bool useGuarded = guarded > 0;
  const char *envUnguarded = getenv("FABRIC_SPLICE_UNGUARDED");
  if ( envUnguarded )
    useGuarded = atoi(envUnguarded) == 0;
  FabricCore::Client * currentClient = (FabricCore::Client*)DGGraphImpl::constructClient(useGuarded, optType);
  if(currentClient == NULL)
    return;
  client = *currentClient;
  //LoggingImpl::log("Constructed FabricSplice Client.");
  FECS_CATCH_VOID
}

FECS_DECL void FECS_setLicenseType(FabricCore::ClientLicenseType licenseType)
{
  FECS_TRY_CLEARERROR
  DGGraphImpl::setLicenseType(licenseType);
  FECS_CATCH_VOID
}

FECS_DECL bool FECS_destroyClient(bool force)
{
  FECS_TRY_CLEARERROR
  return DGGraphImpl::destroyClient(force);
  FECS_CATCH(false)
}

char const * FECS_GetClientContextID()
{
  FECS_TRY_CLEARERROR
  if(!gInitialized)
    return "";
  FabricCore::Client * client = (FabricCore::Client*)DGGraphImpl::getClient();
  if(client == NULL)
    return "";
  return client->getContextID();
  FECS_CATCH("")
}

bool FECS_addExtFolder(const char * folder)
{
  FECS_TRY_CLEARERROR
  return DGGraphImpl::addExtFolder(folder);
  FECS_CATCH(false)
}

FECS_DECL void FECS_setDCCOperatorSourceCodeCallback(FECS_GetOperatorSourceCodeFunc func)
{
  FECS_TRY_CLEARERROR
  return DGGraphImpl::setDCCOperatorSourceCodeCallback(func);
  FECS_CATCH_VOID
}

void FECS_ConstructRTVal(FabricCore::RTVal & result, const char * rt)
{
  FECS_TRY_CLEARERROR
  FECS_ConstructRTValArgs(result, rt, 0, NULL);
  FECS_CATCH_VOID
}

void FECS_ConstructRTValArgs(FabricCore::RTVal & result, const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
  {
    LoggingImpl::reportError("client is null");
    return;
  }

  const char * realExtName = FabricCore::GetRegisteredTypeExtName(*client, rt);
  if(realExtName != NULL)
    DGGraphImpl::loadExtension(realExtName);

  result = FabricCore::RTVal::Construct(*client, rt, nbArgs, args);
  FECS_CATCH_VOID
}

void FECS_ConstructObjectRTVal(FabricCore::RTVal & result, const char * rt)
{
  FECS_TRY_CLEARERROR
  FECS_ConstructObjectRTValArgs(result, rt, 0, NULL);
  FECS_CATCH_VOID
}

void FECS_ConstructObjectRTValArgs(FabricCore::RTVal & result, const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;

  const char * realExtName = FabricCore::GetRegisteredTypeExtName(*client, rt);
  if(realExtName != NULL)
    DGGraphImpl::loadExtension(realExtName);

  result = FabricCore::RTVal::Create(*client, rt, nbArgs, args);
  FECS_CATCH_VOID
}

void FECS_ConstructInterfaceRTValArgs(FabricCore::RTVal & result, const char * rt, const FabricCore::RTVal & object)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;

  const char * realExtName = FabricCore::GetRegisteredTypeExtName(*client, rt);
  if(realExtName != NULL)
    DGGraphImpl::loadExtension(realExtName);

  result = FabricCore::RTVal::Construct(*client, rt, 1, &object);
  FECS_CATCH_VOID
}

void FECS_ConstructBooleanRTVal(FabricCore::RTVal & result, bool value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructBoolean(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructSInt8RTVal(FabricCore::RTVal & result, int8_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructSInt8(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructSInt16RTVal(FabricCore::RTVal & result, int16_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructSInt16(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructSInt32RTVal(FabricCore::RTVal & result, int32_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructSInt32(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructSInt64RTVal(FabricCore::RTVal & result, int64_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructSInt64(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructUInt8RTVal(FabricCore::RTVal & result, uint8_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructUInt8(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructUInt16RTVal(FabricCore::RTVal & result, uint16_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructUInt16(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructUInt32RTVal(FabricCore::RTVal & result, uint32_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructUInt32(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructUInt64RTVal(FabricCore::RTVal & result, uint64_t value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructUInt64(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructFloat32RTVal(FabricCore::RTVal & result, float value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructFloat32(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructFloat64RTVal(FabricCore::RTVal & result, double value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructFloat64(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructDataRTVal(FabricCore::RTVal & result, void *value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructData(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructStringRTVal(FabricCore::RTVal & result, const char * value)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructString(*client, value);
  FECS_CATCH_VOID
}

void FECS_ConstructVariableArrayRTVal(FabricCore::RTVal & result, const char * rt)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructVariableArray(*client, rt);
  FECS_CATCH_VOID
}

void FECS_ConstructExternalArrayRTVal(FabricCore::RTVal & result, const char * rt, uint32_t nbElements, void * data)
{
  FECS_TRY_CLEARERROR
  const FabricCore::Client * client = DGGraphImpl::getClient();
  if(client == NULL)
    return;
  result = FabricCore::RTVal::ConstructExternalArray(*client, rt, nbElements, data);
  FECS_CATCH_VOID
}

const char * FECS_KLParser_owner(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, "")
  return parser->owner();
  FECS_CATCH("");
}

const char * FECS_KLParser_name(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, "")
  return parser->name();
  FECS_CATCH("");
}

const char * FECS_KLParser_code(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, "")
  return parser->code();
  FECS_CATCH("");
}

FECS_DECL unsigned int FECS_KLParser_getNbParsers()
{
  FECS_TRY_CLEARERROR
  return KLParserImpl::getNbParsers();
  FECS_CATCH(0)
}

FECS_DECL FECS_KLParserRef FECS_KLParser_getParserFromIndex(unsigned int index)
{
  FECS_TRY_CLEARERROR
  return new KLParserImplPtr(KLParserImpl::getParser(index));
  FECS_CATCH(NULL)
}

FECS_KLParserRef FECS_KLParser_getParser(const char * owner, const char * name, const char * klCode)
{
  FECS_TRY_CLEARERROR
  return new KLParserImplPtr(KLParserImpl::getParser(owner, name, klCode));
  FECS_CATCH(NULL)
}

FECS_KLParserRef FECS_KLParser_copy(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  KLParserImplPtr * ptr = (KLParserImplPtr *)ref;
  if(ptr == NULL)
    return NULL;
  return new KLParserImplPtr(*ptr);
  FECS_CATCH(NULL);
}

void FECS_KLParser_destroy(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  KLParserImplPtr * ptr = (KLParserImplPtr *)ref;
  if(ptr != NULL)
    delete(ptr);
  FECS_CATCH_VOID
}

bool FECS_KLParser_parse(FECS_KLParserRef ref, const char * klCode)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, false)
  return parser->parse(klCode);
  FECS_CATCH(false);
}

unsigned int FECS_KLParser_getNbKLSymbols(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->nbKLSymbols();
  FECS_CATCH(0);
}

FECS_KLParserSymbolRef FECS_KLParser_getKLSymbol(FECS_KLParserRef ref, unsigned int symbolIndex)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserSymbolRef)parser->getKLSymbol(symbolIndex);
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_getKLSymbolFromCharIndex(FECS_KLParserRef ref, unsigned int charIndex)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserSymbolRef)parser->getKLSymbolFromCharIndex(charIndex);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLRequires(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLRequires();
  FECS_CATCH(0);
}

FECS_DECL const char * FECS_KLParser_getKLRequire(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, "")
  return parser->getKLRequire(index);
  FECS_CATCH("");
}

FECS_DECL unsigned int FECS_KLParser_getNbKLConstants(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLConstants();
  FECS_CATCH(0);
}

FECS_DECL FECS_KLParserConstantRef FECS_KLParser_getKLConstant(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserConstantRef)parser->getKLConstant(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLVariables(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLVariables();
  FECS_CATCH(0);
}

FECS_DECL FECS_KLParserVariableRef FECS_KLParser_getKLVariable(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserVariableRef)parser->getKLVariable(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLInterfaces(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLInterfaces();
  FECS_CATCH(0);
}

FECS_DECL FECS_KLParserInterfaceRef FECS_KLParser_getKLInterface(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserInterfaceRef)parser->getKLInterface(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLStructs(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLStructs();
  FECS_CATCH(0);
}

FECS_DECL FECS_KLParserStructRef FECS_KLParser_getKLStruct(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserStructRef)parser->getKLStruct(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLOperators(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLOperators();
  FECS_CATCH(0);
}

FECS_DECL FECS_KLParserOperatorRef FECS_KLParser_getKLOperator(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserOperatorRef)parser->getKLOperator(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_getNbKLFunctions(FECS_KLParserRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, 0)
  return parser->getNbKLFunctions();
  FECS_CATCH(0);
}

const char * FECS_KLParser_getKLTypeForSymbol(FECS_KLParserRef ref, FECS_KLParserSymbolRef symbol)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, "")
  return parser->getKLTypeForSymbol((const KLParserImpl::KLSymbol *)symbol);
  FECS_CATCH("");
}

const char * FECS_KLParser_getKLTypeForMemberOrMethod(const char * owner, const char * member)
{
  FECS_TRY_CLEARERROR
  return KLParserImpl::getKLTypeForMemberOrMethod(owner, member);
  FECS_CATCH("");
}

FECS_DECL FECS_KLParserFunctionRef FECS_KLParser_getKLFunction(FECS_KLParserRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(KLParserImplPtr, parser, NULL)
  return (FECS_KLParserFunctionRef)parser->getKLFunction(index);
  FECS_CATCH(NULL);
}

FECS_DECL unsigned int FECS_KLParser_KLSymbol_index(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, 0)
  return symbol->index();
  FECS_CATCH(0);
}

FECS_DECL unsigned int FECS_KLParser_KLSymbol_pos(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, 0)
  return symbol->pos();
  FECS_CATCH(0);
}

FECS_DECL unsigned int FECS_KLParser_KLSymbol_length(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, 0)
  return symbol->length();
  FECS_CATCH(0);
}

FECS_DECL int FECS_KLParser_KLSymbol_type(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, 0)
  return (int)symbol->type();
  FECS_CATCH(0);
}

FECS_DECL bool FECS_KLParser_KLSymbol_isKeyword(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, false)
  return symbol->isKeyword();
  FECS_CATCH(false);
}

FECS_DECL const char * FECS_KLParser_KLSymbol_typeName(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, "")
  return symbol->typeName();
  FECS_CATCH("");
}

FECS_DECL char FECS_KLParser_KLSymbol_front(FECS_KLParserSymbolRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, '\0')
  return symbol->front(index);
  FECS_CATCH('\0');
}

FECS_DECL char FECS_KLParser_KLSymbol_back(FECS_KLParserSymbolRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, '\0')
  return symbol->back(index);
  FECS_CATCH('\0');
}

FECS_DECL char * FECS_KLParser_KLSymbol_str(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, NULL)
  std::string s = symbol->str();
  char * result = (char*)malloc(s.length() + 1);
  result[s.length()] = '\0';
  memcpy(result, s.c_str(), s.length());
  return result;
  FECS_CATCH(NULL);
}

FECS_DECL bool FECS_KLParser_KLSymbol_contains(FECS_KLParserSymbolRef ref, char c)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, false)
  return symbol->contains(c);
  FECS_CATCH(false);
}

FECS_DECL unsigned int FECS_KLParser_KLSymbol_find(FECS_KLParserSymbolRef ref, char c)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, UINT_MAX)
  return symbol->find(c);
  FECS_CATCH(UINT_MAX);
}

FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLSymbol_prev(FECS_KLParserSymbolRef ref, bool skipComments, unsigned int offset)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, NULL)
  return (FECS_KLParserSymbolRef)symbol->prev(skipComments, offset);
  FECS_CATCH(NULL);
}

FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLSymbol_next(FECS_KLParserSymbolRef ref, bool skipComments, unsigned int offset)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, NULL)
  return (FECS_KLParserSymbolRef)symbol->next(skipComments, offset);
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLSymbol_parser(FECS_KLParserSymbolRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLSymbol, symbol, "")
  return symbol->parser();
  FECS_CATCH("");
}

FECS_KLParserSymbolRef FECS_KLParser_KLConstant_symbol(FECS_KLParserConstantRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLConstant, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLConstant_comments(FECS_KLParserConstantRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLConstant, c, "")
  return c->comments();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLConstant_type(FECS_KLParserConstantRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLConstant, c, "")
  return c->type();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLConstant_name(FECS_KLParserConstantRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLConstant, c, "")
  return c->name();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLConstant_value(FECS_KLParserConstantRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLConstant, c, "")
  return c->value();
  FECS_CATCH("");
}

FECS_KLParserSymbolRef FECS_KLParser_KLVariable_symbol(FECS_KLParserVariableRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLVariable, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLVariable_type(FECS_KLParserVariableRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLVariable, c, "")
  return c->type();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLVariable_name(FECS_KLParserVariableRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLVariable, c, "")
  return c->name();
  FECS_CATCH("");
}

FECS_KLParserSymbolRef FECS_KLParser_KLStruct_symbol(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLStruct_comments(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->comments();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLStruct_type(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->type();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLStruct_name(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->name();
  FECS_CATCH("");
}

unsigned int FECS_KLParser_KLStruct_nbInterfaces(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, 0)
  return c->nbInterfaces();
  FECS_CATCH(0);
}

const char * FECS_KLParser_KLStruct_interface(FECS_KLParserStructRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->interface(index);
  FECS_CATCH("");
}

unsigned int FECS_KLParser_KLStruct_nbMembers(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, 0)
  return c->nbMembers();
  FECS_CATCH(0);
}

const char * FECS_KLParser_KLStruct_memberType(FECS_KLParserStructRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->memberType(index);
  FECS_CATCH("");
}

const char * FECS_KLParser_KLStruct_memberName(FECS_KLParserStructRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLStruct, c, "")
  return c->memberName(index);
  FECS_CATCH("");
}

unsigned int FECS_KLParser_KLArgumentList_nbArgs(FECS_KLParserStructRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLArgumentList, c, 0)
  return c->nbArgs();
  FECS_CATCH(0);
}

const char * FECS_KLParser_KLArgumentList_mode(FECS_KLParserArgumentListRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLArgumentList, c, "")
  return c->mode(index);
  FECS_CATCH("");
}

const char * FECS_KLParser_KLArgumentList_type(FECS_KLParserArgumentListRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLArgumentList, c, "")
  return c->type(index);
  FECS_CATCH("");
}

const char * FECS_KLParser_KLArgumentList_name(FECS_KLParserArgumentListRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLArgumentList, c, "")
  return c->name(index);
  FECS_CATCH("");
}

FECS_KLParserSymbolRef FECS_KLParser_KLOperator_symbol(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLOperator_comments(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, "")
  return c->comments();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLOperator_name(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, "")
  return c->name();
  FECS_CATCH("");
}

bool FECS_KLParser_KLOperator_isPex(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, false)
  return c->isPex();
  FECS_CATCH(false);
}

const char * FECS_KLParser_KLOperator_pexArgument(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, "")
  return c->pexArgument();
  FECS_CATCH("");
}

FECS_KLParserArgumentListRef FECS_KLParser_KLOperator_arguments(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, NULL)
  return (FECS_KLParserArgumentListRef)c->arguments();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLOperator_bodyStart(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, NULL)
  return (FECS_KLParserSymbolRef)c->bodyStart();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLOperator_bodyEnd(FECS_KLParserOperatorRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLOperator, c, NULL)
  return (FECS_KLParserSymbolRef)c->bodyEnd();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLFunction_symbol(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLFunction_comments(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, "")
  return c->comments();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLFunction_type(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, "")
  return c->type();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLFunction_owner(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, "")
  return c->owner();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLFunction_name(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, "")
  return c->name();
  FECS_CATCH("");
}

FECS_KLParserArgumentListRef FECS_KLParser_KLFunction_arguments(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, NULL)
  return (FECS_KLParserArgumentListRef)c->arguments();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLFunction_bodyStart(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, NULL)
  return (FECS_KLParserSymbolRef)c->bodyStart();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLFunction_bodyEnd(FECS_KLParserFunctionRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLFunction, c, NULL)
  return (FECS_KLParserSymbolRef)c->bodyEnd();
  FECS_CATCH(NULL);
}

FECS_KLParserSymbolRef FECS_KLParser_KLInterface_symbol(FECS_KLParserInterfaceRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLInterface, c, NULL)
  return (FECS_KLParserSymbolRef)c->symbol();
  FECS_CATCH(NULL);
}

const char * FECS_KLParser_KLInterface_comments(FECS_KLParserInterfaceRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLInterface, c, "")
  return c->comments();
  FECS_CATCH("");
}

const char * FECS_KLParser_KLInterface_name(FECS_KLParserInterfaceRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLInterface, c, "")
  return c->name();
  FECS_CATCH("");
}

unsigned int FECS_KLParser_KLInterface_nbFunctions(FECS_KLParserInterfaceRef ref)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLInterface, c, 0)
  return c->nbFunctions();
  FECS_CATCH(0);
}

FECS_KLParserFunctionRef FECS_KLParser_KLInterface_function(FECS_KLParserInterfaceRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETPTR(KLParserImpl::KLInterface, c, NULL)
  return (FECS_KLParserFunctionRef)c->function(index);
  FECS_CATCH(NULL);
}

void FECS_Logging_setLogFunc(FECS_LoggingFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setLogFunc(func);
  FECS_CATCH_VOID
}

void FECS_Logging_setLogErrorFunc(FECS_LoggingFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setLogErrorFunc(func); 
  FECS_CATCH_VOID
}

void FECS_Logging_setCompilerErrorFunc(FECS_CompilerErrorFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setCompilerErrorFunc(func); 
  FECS_CATCH_VOID
}

void FECS_Logging_setKLReportFunc(FECS_LoggingFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setKLReportFunc(func); 
  FECS_CATCH_VOID
}

void FECS_Logging_setKLStatusFunc(FECS_StatusFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setKLStatusFunc(func); 
  FECS_CATCH_VOID
}

void FECS_Logging_setSlowOperationFunc(FECS_SlowOperationFunc func)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::setSlowOperationFunc(func); 
  FECS_CATCH_VOID
}

bool FECS_Logging_hasError()
{
  FECS_TRY
  return LoggingImpl::hasError();
  FECS_CATCH(false);
}

char const * FECS_Logging_getError()
{
  FECS_TRY
  return LoggingImpl::getError();
  FECS_CATCH("");
}

void FECS_Logging_clearError()
{
  FECS_TRY_CLEARERROR
  FECS_CATCH_VOID
}

void FECS_Logging_enableTimers()
{
  FECS_TRY_CLEARERROR
  LoggingImpl::enableTimers();
  FECS_CATCH_VOID
}

void FECS_Logging_disableTimers()
{
  FECS_TRY_CLEARERROR
  LoggingImpl::disableTimers();
  FECS_CATCH_VOID
}

void FECS_Logging_resetTimer(const char * name)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::resetTimer(name);
  FECS_CATCH_VOID
}

void FECS_Logging_startTimer(const char * name)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::startTimer(name);
  FECS_CATCH_VOID
}

void FECS_Logging_stopTimer(const char * name)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::stopTimer(name);
  FECS_CATCH_VOID
}

void FECS_Logging_logTimer(const char * name)
{
  FECS_TRY_CLEARERROR
  LoggingImpl::logTimer(name);
  FECS_CATCH_VOID
}

unsigned int FECS_Logging_getNbTimers()
{
  FECS_TRY
  return LoggingImpl::getNbTimers();
  FECS_CATCH(0);
}

char const * FECS_Logging_getTimerName(unsigned int index)
{
  FECS_TRY_CLEARERROR
  return LoggingImpl::getTimerName(index);
  FECS_CATCH("")
}

// void FECS_SceneManagement_setManipulationFunc(FECS_ManipulationFunc func)
// {
//   FECS_TRY_CLEARERROR
//   SceneManagementImpl::setManipulationFunc((ManipulationFunc)(void*)func);
//   FECS_CATCH_VOID
// }

bool FECS_SceneManagement_hasRenderableContent()
{
  FECS_TRY_CLEARERROR
  return SceneManagementImpl::hasRenderableContent();
  FECS_CATCH(false);
}

void FECS_SceneManagement_drawOpenGL(FabricCore::RTVal & drawContext)
{
  FECS_TRY_CLEARERROR
  SceneManagementImpl::drawOpenGL(drawContext);
  FECS_CATCH_VOID
}

// bool FECS_SceneManagement_raycast(FabricCore::RTVal & raycastContext, FECS_DGPortRef & portRef)
// {
//   FECS_TRY_CLEARERROR
//   portRef = NULL;
//   LoggingImpl::clearError();
//   DGPortImplPtr port;
//   bool result = SceneManagementImpl::raycast(raycastContext, port);
//   if(port)
//     portRef = new DGPortImplPtr(port);
//   return result;
//   FECS_CATCH(false);
// }

void FECS_Scripting_parseScriptingArguments(const char * action, const char * reference, const char * data, const char * auxiliary, FabricCore::Variant & result)
{
  FECS_TRY_CLEARERROR

  FabricCore::Variant argsArray = FabricCore::Variant::CreateArray();
  argsArray.arrayAppend(FabricCore::Variant::CreateString(action));
  argsArray.arrayAppend(FabricCore::Variant::CreateString(reference));
  argsArray.arrayAppend(FabricCore::Variant::CreateString(data));
  argsArray.arrayAppend(FabricCore::Variant::CreateString(auxiliary));

  result = FabricCore::Variant::CreateDict();
  if(!argsArray.isArray())
    return;
  if(argsArray.getArraySize() == 0)
    return;
  unsigned int elementId = 0;
  const FabricCore::Variant * element = argsArray.getArrayElement(elementId++);
  if(!element->isString())
    return;
  std::string dataStr = element->getStringData();
  std::string dataStrLower = dataStr;
  std::transform(dataStrLower.begin(), dataStrLower.end(), dataStrLower.begin(), ::tolower);

  result.setDictValue("action", FabricCore::Variant::CreateString(dataStr.c_str()));
  if(argsArray.getArraySize() == elementId)
    return;

  element = argsArray.getArrayElement(elementId++);
  if(element->isString())
  {
    dataStr = element->getStringData();
    if(dataStr.length() > 0)
      result.setDictValue("reference", FabricCore::Variant::CreateString(dataStr.c_str()));
    if(argsArray.getArraySize() == elementId)
      return;
  }

  element = argsArray.getArrayElement(elementId++);
  if(element->isString())
  {
    dataStr = element->getStringData();
    if(dataStr.length() > 0)
    {
      FabricCore::Variant jsonVar;
      try
      {
        jsonVar = FabricCore::Variant::CreateFromJSON(dataStr.c_str());
      }
      catch(FabricCore::Exception e)
      {
        LoggingImpl::logError("Scripting data argument not a valid JSON string.");
        return;
      }
      if(!jsonVar.isDict())
      {
        LoggingImpl::logError("Scripting data argument not a valid JSON dictionary.");
        return;
      }
      for(FabricCore::Variant::DictIter keyIter(jsonVar); !keyIter.isDone(); keyIter.next())
      {
        std::string key = keyIter.getKey()->getStringData();
        result.setDictValue(key.c_str(), *keyIter.getValue());      
      }
    }
    if(argsArray.getArraySize() == elementId)
      return;
  }

  element = argsArray.getArrayElement(elementId++);
  if(element->isString())
  {
    dataStr = element->getStringData();
    if(dataStr.length() > 0)
      result.setDictValue("auxiliary", FabricCore::Variant::CreateString(dataStr.c_str()));
    if(argsArray.getArraySize() == elementId)
      return;
  }
  FECS_CATCH_VOID
}

bool FECS_Scripting_consumeBooleanArgument(FabricCore::Variant & argsDict, const char * name, bool defaultValue, bool optional)
{
  FECS_TRY_CLEARERROR
  if(!argsDict.isDict())
    return defaultValue;
  const FabricCore::Variant * elementVar = argsDict.getDictValue(name);
  if(!elementVar)
  {
    if(!optional)
    {
      std::string errorMessage = "Boolean script argument '";
      errorMessage += name;
      errorMessage += "' missing.";
      LoggingImpl::logError(errorMessage);
    }
    return defaultValue;
  }
  if(!elementVar->isBoolean())
    return defaultValue;
  bool result = elementVar->getBoolean();
  argsDict.setDictValue(name, FabricCore::Variant());
  return result;
  FECS_CATCH(false);
}

int FECS_Scripting_consumeIntegerArgument(FabricCore::Variant & argsDict, const char * name, int defaultValue, bool optional)
{
  FECS_TRY_CLEARERROR
  if(!argsDict.isDict())
    return defaultValue;
  const FabricCore::Variant * elementVar = argsDict.getDictValue(name);
  if(!elementVar)
  {
    if(!optional)
    {
      std::string errorMessage = "Integer script argument '";
      errorMessage += name;
      errorMessage += "' missing.";
      LoggingImpl::logError(errorMessage);
    }
    return defaultValue;
  }
  int result = defaultValue;
  if(elementVar->isUInt8())
    result = (int)elementVar->getUInt8();
  if(elementVar->isUInt16())
    result = (int)elementVar->getUInt16();
  if(elementVar->isUInt32())
    result = (int)elementVar->getUInt32();
  if(elementVar->isSInt8())
    result = (int)elementVar->getSInt8();
  if(elementVar->isSInt16())
    result = (int)elementVar->getSInt16();
  if(elementVar->isSInt32())
    result = (int)elementVar->getSInt32();
  if(elementVar->isFloat32())
    result = (int)elementVar->getFloat32();
  if(elementVar->isFloat64())
    result = (int)elementVar->getFloat64();
  argsDict.setDictValue(name, FabricCore::Variant());
  return result;
  FECS_CATCH(defaultValue);
}

float FECS_Scripting_consumeScalarArgument(FabricCore::Variant & argsDict, const char * name, float defaultValue, bool optional)
{
  FECS_TRY_CLEARERROR
  if(!argsDict.isDict())
    return defaultValue;
  const FabricCore::Variant * elementVar = argsDict.getDictValue(name);
  if(!elementVar)
  {
    if(!optional)
    {
      std::string errorMessage = "Scalar script argument '";
      errorMessage += name;
      errorMessage += "' missing.";
      LoggingImpl::logError(errorMessage);
    }
    return defaultValue;
  }
  float result = defaultValue;
  if(elementVar->isUInt8())
    result = (float)elementVar->getUInt8();
  else if(elementVar->isUInt16())
    result = (float)elementVar->getUInt16();
  else if(elementVar->isUInt32())
    result = (float)elementVar->getUInt32();
  else if(elementVar->isSInt8())
    result = (float)elementVar->getSInt8();
  else if(elementVar->isSInt16())
    result = (float)elementVar->getSInt16();
  else if(elementVar->isSInt32())
    result = (float)elementVar->getSInt32();
  else if(elementVar->isFloat32())
    result = (float)elementVar->getFloat32();
  else if(elementVar->isFloat64())
    result = (float)elementVar->getFloat64();
  argsDict.setDictValue(name, FabricCore::Variant());
  return result;
  FECS_CATCH(defaultValue);
}

char * FECS_Scripting_consumeStringArgument(FabricCore::Variant & argsDict, const char * name, const char * defaultValue, bool optional)
{
  FECS_TRY_CLEARERROR
  if(!argsDict.isDict())
    return NULL;
  const FabricCore::Variant * elementVar = argsDict.getDictValue(name);
  if(!elementVar)
  {
    if(!optional)
    {
      std::string errorMessage = "String script argument '";
      errorMessage += name;
      errorMessage += "' missing.";
      LoggingImpl::logError(errorMessage);
    }
    return NULL;
  }
  if(!elementVar->isString())
    return NULL;
  std::string result = elementVar->getStringData();
  char * resultPtr = (char*)malloc(result.length()+1);
  resultPtr[result.length()] = '\0';
  memcpy(resultPtr, result.c_str(), result.length());
  return resultPtr;
  FECS_CATCH(NULL);
}

void FECS_Scripting_consumeVariantArgument(FabricCore::Variant & argsDict, const char * name, const FabricCore::Variant & defaultValue, bool optional, FabricCore::Variant & result)
{
  FECS_TRY_CLEARERROR
  if(!argsDict.isDict())
  {
    result = defaultValue;
    return;
  }
  const FabricCore::Variant * elementVar = argsDict.getDictValue(name);
  if(!elementVar)
  {
    if(!optional)
    {
      std::string errorMessage = "Variant script argument '";
      errorMessage += name;
      errorMessage += "' missing.";
      LoggingImpl::logError(errorMessage);
    }
    result = defaultValue;
    return;
  }
  result = *elementVar;
  argsDict.setDictValue(name, FabricCore::Variant());
  FECS_CATCH_VOID
}

FECS_DGGraphRef FECS_DGGraph_construct(const char * name, int guarded, FabricCore::ClientOptimizationType optType)
{
  FECS_TRY_CLEARERROR
  bool useGuarded = guarded > 0;
  const char *envUnguarded = getenv("FABRIC_SPLICE_UNGUARDED");
  if ( envUnguarded )
    useGuarded = atoi(envUnguarded) == 0;
  return new DGGraphImplPtr(DGGraphImpl::construct(name, useGuarded, optType));
  FECS_CATCH(NULL);
}

FECS_DGGraphRef FECS_DGGraph_copy(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  DGGraphImplPtr * ptr = (DGGraphImplPtr *)ref;
  if(ptr == NULL)
    return NULL;
  return new DGGraphImplPtr(*ptr);
  FECS_CATCH(NULL);
}

void FECS_DGGraph_destroy(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  DGGraphImplPtr * ptr = (DGGraphImplPtr *)ref;
  if(ptr != NULL)
    delete(ptr);
  FECS_CATCH_VOID
}

void FECS_DGGraph_clear(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->clear();
  FECS_CATCH_VOID
}

void FECS_DGGraph_getClient(const FabricCore::Client ** client)
{
  FECS_TRY_CLEARERROR
  *client = DGGraphImpl::getClient();
  FECS_CATCH_VOID
}

void FECS_DGGraph_loadExtension(const char *extensionName)
{
  FECS_TRY_CLEARERROR
  DGGraphImpl::loadExtension(extensionName);
  FECS_CATCH_VOID
}

void * FECS_DGGraph_getUserPointer(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  return graph->getUserPointer();
  FECS_CATCH(NULL);
}

void FECS_DGGraph_setUserPointer(FECS_DGGraphRef ref, void * data)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->setUserPointer(data);
  FECS_CATCH_VOID
}

char const * FECS_DGGraph_getName(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getName_cstr();
  FECS_CATCH("")
}

bool FECS_DGGraph_setName(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setName(name);
  FECS_CATCH(false);
}

char const * FECS_DGGraph_getMetaData(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getMetaData().c_str();
  FECS_CATCH("")
}

void FECS_DGGraph_setMetaData(FECS_DGGraphRef ref, const char * json)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->setMetaData(json);
  FECS_CATCH_VOID
}

unsigned int FECS_DGGraph_getDGNodeCount(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return (unsigned int)graph->getDGNodeCount();
  FECS_CATCH(0);
}

char const * FECS_DGGraph_getDGNodeName(FECS_DGGraphRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getDGNodeName(index);
  FECS_CATCH("")
}

bool FECS_DGGraph_hasDGNode(FECS_DGGraphRef ref, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->hasDGNode(dgNodeName);
  FECS_CATCH(false);
}

void FECS_DGGraph_getDGNode(FECS_DGGraphRef ref, FabricCore::DGNode & dgNode, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  dgNode = graph->getDGNode(dgNodeName);
  FECS_CATCH_VOID
}

void FECS_DGGraph_getDGNodeByIndex(FECS_DGGraphRef ref, FabricCore::DGNode & dgNode, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  dgNode = graph->getDGNode(index);
  FECS_CATCH_VOID
}

void FECS_DGGraph_constructDGNode(FECS_DGGraphRef ref, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->constructDGNode(dgNodeName);
  FECS_CATCH_VOID
}

void FECS_DGGraph_removeDGNode(FECS_DGGraphRef ref, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->removeDGNode(dgNodeName);
  FECS_CATCH_VOID
}

bool FECS_DGGraph_addDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * rt, FabricCore::Variant defaultValue, const char * dgNodeName, const char * extension)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->addDGNodeMember(name, rt, defaultValue, dgNodeName, extension);
  FECS_CATCH(false);
}

bool FECS_DGGraph_hasDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->hasDGNodeMember(name, dgNodeName);
  FECS_CATCH(false);
}

bool FECS_DGGraph_removeDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->removeDGNodeMember(name, dgNodeName);
  FECS_CATCH(false);
}

void FECS_DGGraph_generateKLOperatorParameterList(FECS_DGGraphRef ref, FabricCore::Variant & code)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  std::string codeStr = graph->generateKLOperatorParameterList();
  code = FabricCore::Variant::CreateString(codeStr.c_str());
  FECS_CATCH_VOID
}

void FECS_DGGraph_generateKLOperatorSourceCode(FECS_DGGraphRef ref, FabricCore::Variant & code, const char * name, const char * additionalBody, const char * additionalFunctions, const char * executeParallelMember)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  std::string codeStr = graph->generateKLOperatorSourceCode(name, additionalBody, additionalFunctions, executeParallelMember);
  code = FabricCore::Variant::CreateString(codeStr.c_str());
  FECS_CATCH_VOID
}

bool FECS_DGGraph_constructKLOperator(FECS_DGGraphRef ref, const char * name, const char * sourceCode, const char * entry, const char * dgNodeName, const FabricCore::Variant & portMap)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->constructKLOperator(name, sourceCode, entry, dgNodeName, portMap);
  FECS_CATCH(false);
}

bool FECS_DGGraph_removeKLOperator(FECS_DGGraphRef ref, const char * name, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->removeKLOperator(name, dgNodeName);
  FECS_CATCH(false);
}

bool FECS_DGGraph_hasKLOperator(FECS_DGGraphRef ref, const char * name, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->hasKLOperator(name, dgNodeName);
  FECS_CATCH(false);
}

char const * FECS_DGGraph_getKLOperatorEntry(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getKLOperatorEntry(name);
  FECS_CATCH("")
}

bool FECS_DGGraph_setKLOperatorEntry(FECS_DGGraphRef ref, const char * name, const char * entry)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setKLOperatorEntry(name, entry);
  FECS_CATCH(false);
}

bool FECS_DGGraph_setKLOperatorIndex(FECS_DGGraphRef ref, const char * name, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setKLOperatorIndex(name, index);
  FECS_CATCH(false);
}

char const * FECS_DGGraph_getKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getKLOperatorSourceCode(name);
  FECS_CATCH("")
}

bool FECS_DGGraph_setKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * sourceCode, const char * entry)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setKLOperatorSourceCode(name, sourceCode, entry);
  FECS_CATCH(false);
}

void FECS_DGGraph_loadKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * filePath)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->loadKLOperatorSourceCode(name, filePath);
  FECS_CATCH_VOID
}

void FECS_DGGraph_saveKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * filePath)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->saveKLOperatorSourceCode(name, filePath);
  FECS_CATCH_VOID
}

bool FECS_DGGraph_isKLOperatorFileBased(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->isKLOperatorFileBased(name);
  FECS_CATCH(false);
}

char const * FECS_DGGraph_getKLOperatorFilePath(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->getKLOperatorFilePath(name);
  FECS_CATCH("");
}

void FECS_DGGraph_setKLOperatorFilePath(FECS_DGGraphRef ref, const char * name, const char * filePath, const char * entry)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->setKLOperatorFilePath(name, filePath, entry);
  FECS_CATCH_VOID
}

unsigned int FECS_DGGraph_getKLOperatorCount(FECS_DGGraphRef ref, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->getKLOperatorCount(dgNodeName);
  FECS_CATCH(0);
}

char const * FECS_DGGraph_getKLOperatorName(FECS_DGGraphRef ref, unsigned int index, const char * dgNodeName)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getKLOperatorName(index, dgNodeName);
  FECS_CATCH("")
}

unsigned int FECS_DGGraph_getGlobalKLOperatorCount()
{
  FECS_TRY_CLEARERROR
  return (unsigned int)DGGraphImpl::getGlobalKLOperatorCount();
  FECS_CATCH(0);
}

char const * FECS_DGGraph_getGlobalKLOperatorName(unsigned int index)
{
  FECS_TRY_CLEARERROR
  return DGGraphImpl::getGlobalKLOperatorName(index);
  FECS_CATCH("")
}

bool FECS_DGGraph_checkErrors()
{
  FECS_TRY_CLEARERROR
  return DGGraphImpl::checkErrors();
  FECS_CATCH(false);
}

bool FECS_DGGraph_evaluate(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->evaluate();
  FECS_CATCH(false);
}

void FECS_DGGraph_setEvaluateShared(FECS_DGGraphRef ref, bool evaluateShared)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->setEvaluateShared(evaluateShared);
  FECS_CATCH_VOID;
}

bool FECS_DGGraph_clearEvaluate(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->clearEvaluate();
  FECS_CATCH(false);
}

bool FECS_DGGraph_usesEvalContext(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->usesEvalContext();
  FECS_CATCH(false);
}

bool FECS_DGGraph_requireEvaluate(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->requireEvaluate();
  FECS_CATCH(false);
}

FECS_DECL void FECS_DGGraph_getEvalContext(FECS_DGPortRef ref, FabricCore::RTVal & result)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  result = graph->getEvalContext();
  FECS_CATCH_VOID
}

FECS_DGPortRef FECS_DGGraph_addDGPort(FECS_DGGraphRef ref, const char * name, const char * member, FECS_DGPort_Mode mode, const char * dgNodeName, bool autoInitObjects)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  DGPortImplPtr port = graph->addDGPort(graph, name, member, DGPortImpl::Mode(mode), dgNodeName, autoInitObjects);
  if(!port)
    return NULL;
  return new DGPortImplPtr(port);
  FECS_CATCH(NULL);
}

bool FECS_DGGraph_removeDGPort(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->removeDGPort(name);
  FECS_CATCH(false);
}

FECS_DGPortRef FECS_DGGraph_getDGPort(FECS_DGGraphRef ref, const char * name)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  DGPortImplPtr port = graph->getDGPort(name);
  if(!port)
    return NULL;
  return new DGPortImplPtr(port);
  FECS_CATCH(NULL);
}

FECS_DGPortRef FECS_DGGraph_getDGPortByIndex(FECS_DGGraphRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  DGPortImplPtr port = graph->getDGPort(index);
  if(!port)
    return NULL;
  return new DGPortImplPtr(port);
  FECS_CATCH(NULL);
}

unsigned int FECS_DGGraph_getDGPortCount(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->getDGPortCount();
  FECS_CATCH(0);
}

char const * FECS_DGGraph_getDGPortName(FECS_DGGraphRef ref, unsigned int index)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getDGPortName(index);
  FECS_CATCH("")
}

char * FECS_DGGraph_getDGPortInfo(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  FabricCore::Variant portInfo;
  graph->getDGPortInfo(portInfo);
  std::string json = portInfo.getJSONEncoding().getStringData();
  char * result = (char*)malloc(json.length()+1);
  result[json.length()] = '\0';
  memcpy(result, json.c_str(), json.length());
  return result;
  FECS_CATCH(NULL)
}

bool FECS_DGGraph_hasDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->hasDGNodeDependency(dgNode, dependency);
  FECS_CATCH(false);
}

bool FECS_DGGraph_setDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->setDGNodeDependency(dgNode, dependency);
  FECS_CATCH(false);
}

bool FECS_DGGraph_removeDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, 0)
  return graph->removeDGNodeDependency(dgNode, dependency);
  FECS_CATCH(false);
}


void FECS_DGGraph_getPersistenceDataDict(FECS_DGGraphRef ref, FabricCore::Variant & dict, const FECS_PersistenceInfo * info)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  dict = graph->getPersistenceDataDict((const DGGraphImpl::PersistenceInfo *)info);
  FECS_CATCH_VOID
}

char * FECS_DGGraph_getPersistenceDataJSON(FECS_DGGraphRef ref, const FECS_PersistenceInfo * info)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, NULL)
  std::string json = graph->getPersistenceDataJSON((const DGGraphImpl::PersistenceInfo *)info);
  char * result = (char*)malloc(json.length()+1);
  result[json.length()] = '\0';
  memcpy(result, json.c_str(), json.length());
  return result;
  FECS_CATCH(NULL)
}

bool FECS_DGGraph_setFromPersistenceDataDict(FECS_DGGraphRef ref, const FabricCore::Variant & dict, FECS_PersistenceInfo * info, const char * baseFilePath)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setFromPersistenceDataDict(graph, dict, (DGGraphImpl::PersistenceInfo *)info, baseFilePath);
  FECS_CATCH(false);
}

bool FECS_DGGraph_setFromPersistenceDataJSON(FECS_DGGraphRef ref, const char * json, FECS_PersistenceInfo * info, const char * baseFilePath)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->setFromPersistenceDataJSON(graph, json, (DGGraphImpl::PersistenceInfo *)info, baseFilePath);
  FECS_CATCH(false);
}

bool FECS_DGGraph_saveToFile(FECS_DGGraphRef ref, const char * filePath, const FECS_PersistenceInfo * info)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->saveToFile(filePath, (const DGGraphImpl::PersistenceInfo *)info);
  FECS_CATCH(false);
}

bool FECS_DGGraph_loadFromFile(FECS_DGGraphRef ref, const char * filePath, FECS_PersistenceInfo * info, bool asReferenced)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->loadFromFile(graph, filePath, (DGGraphImpl::PersistenceInfo *)info, asReferenced);
  FECS_CATCH(false);
}

bool FECS_DGGraph_reloadFromFile(FECS_DGGraphRef ref, FECS_PersistenceInfo * info)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->reloadFromFile(graph, (DGGraphImpl::PersistenceInfo *)info);
  FECS_CATCH(false);
}

FECS_DECL bool FECS_DGGraph_isReferenced(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, false)
  return graph->isReferenced();
  FECS_CATCH(false);
}

FECS_DECL char const * FECS_DGGraph_getReferencedFilePath(FECS_DGGraphRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGGraphImplPtr, graph, "")
  return graph->getReferencedFilePath();
  FECS_CATCH("");
}

FECS_DECL void FECS_DGGraph_setMemberPersistence(FECS_DGGraphRef ref, const char * name, bool persistence)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGGraphImplPtr, graph)
  graph->setMemberPersistence(name, persistence);
  FECS_CATCH_VOID;
}

FECS_DGPortRef FECS_DGPort_copy(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  DGPortImplPtr * ptr = (DGPortImplPtr *)ref;
  if(ptr == NULL)
    return NULL;
  return new DGPortImplPtr(*ptr);
  FECS_CATCH(NULL);
}

void FECS_DGPort_destroy(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  DGPortImplPtr * ptr = (DGPortImplPtr *)ref;
  if(ptr != NULL)
    delete(ptr);
  FECS_CATCH_VOID
}

unsigned int FECS_DGPort_getMode(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, FECS_DGPort_Mode_IO)
  return (unsigned int)port->getMode();
  FECS_CATCH(0);
}

void FECS_DGPort_setMode(FECS_DGPortRef ref, unsigned int mode)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  port->setMode((DGPortImpl::Mode)mode);
  FECS_CATCH_VOID
}

char const * FECS_DGPort_getName(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, "")
  return port->getName_cstr();
  FECS_CATCH("")
}

char const * FECS_DGPort_getMember(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, "")
  return port->getMember();
  FECS_CATCH("")
}

char const * FECS_DGPort_getDGNodeName(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, "")
  return port->getDGNodeName();
  FECS_CATCH("")
}

char const * FECS_DGPort_getKey(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, "")
  return port->getKey();
  FECS_CATCH("")
}

char const * FECS_DGPort_getDataType(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, "")
  return port->getDataType();
  FECS_CATCH("")
}

unsigned int FECS_DGPort_getDataSize(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return (unsigned int)port->getDataSize();
  FECS_CATCH(0);
}

bool FECS_DGPort_isShallow(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->isShallow();
  FECS_CATCH(false);
}

bool FECS_DGPort_isArray(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->isArray();
  FECS_CATCH(false);
}

bool FECS_DGPort_isStruct(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->isStruct();
  FECS_CATCH(false);
}

bool FECS_DGPort_isObject(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->isObject();
  FECS_CATCH(false);
}

bool FECS_DGPort_isInterface(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->isInterface();
  FECS_CATCH(false);
}

bool FECS_DGPort_doesAutoInitObjects(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->doesAutoInitObjects();
  FECS_CATCH(false);
}

unsigned int FECS_DGPort_getSliceCount(FECS_DGPortRef ref)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return (unsigned int)port->getSliceCount();
  FECS_CATCH(0);
}

bool FECS_DGPort_setSliceCount(FECS_DGPortRef ref, unsigned int count)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setSliceCount((uint32_t)count);
  FECS_CATCH(false);
}

void FECS_DGPort_getVariant(FECS_DGPortRef ref, unsigned int slice, FabricCore::Variant & result)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  result = port->getVariant(slice);
  FECS_CATCH_VOID
}

bool FECS_DGPort_setVariant(FECS_DGPortRef ref, const FabricCore::Variant & value, unsigned int slice)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setVariant(value, slice);
  FECS_CATCH(false);
}

char * FECS_DGPort_getJSON(FECS_DGPortRef ref, unsigned int slice)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, NULL)
  std::string json = port->getJSON(slice);
  char * result = (char*)malloc(json.length()+1);
  result[json.length()] = '\0';
  memcpy(result, json.c_str(), json.length());
  return result;
  FECS_CATCH(NULL)
}

bool FECS_DGPort_setJSON(FECS_DGPortRef ref, const char * json, unsigned int slice)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setJSON(json, slice);
  FECS_CATCH(false);
}

void FECS_DGPort_getDefault(FECS_DGPortRef ref, FabricCore::Variant & result)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  result = port->getDefault();
  FECS_CATCH_VOID
}

void FECS_DGPort_getRTVal(
  FECS_DGPortRef ref,
  bool evaluate,
  unsigned int slice,
  FabricCore::RTVal & result
  )
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  result = port->getRTVal(evaluate, slice);
  FECS_CATCH_VOID
}

bool FECS_DGPort_setRTVal(
  FECS_DGPortRef ref,
  const FabricCore::RTVal & value,
  unsigned int slice
  )
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setRTVal(value, slice);
  FECS_CATCH(false);
}

unsigned int FECS_DGPort_getArrayCount(FECS_DGPortRef ref, unsigned int slice)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, 0)
  return port->getArrayCount(slice);
  FECS_CATCH(0);
}

bool FECS_DGPort_getArrayData(
  FECS_DGPortRef ref,
  void * buffer,
  unsigned int bufferSize,
  unsigned int slice
  )
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->getArrayData(buffer, bufferSize, slice);
  FECS_CATCH(false);
}

bool FECS_DGPort_setArrayData(
  FECS_DGPortRef ref,
  void * buffer,
  unsigned int bufferSize,
  unsigned int slice
  )
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setArrayData(buffer, bufferSize, slice);
  FECS_CATCH(false);
}

bool FECS_DGPort_getAllSlicesData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->getAllSlicesData(buffer, bufferSize);
  FECS_CATCH(false);
}

bool FECS_DGPort_setAllSlicesData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  return port->setAllSlicesData(buffer, bufferSize);
  FECS_CATCH(false);
}

bool FECS_DGPort_copyArrayDataFromPort(FECS_DGPortRef ref, FECS_DGPortRef otherRef, unsigned int slice, unsigned int otherSlice)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  DGPortImplPtr * otherPtr = (DGPortImplPtr *)otherRef;
  if(!otherPtr)
  {
    LoggingImpl::logError("Other port ref is NULL!");
    return false;
  }
  DGPortImplPtr & otherPort = *otherPtr;
  return port->copyArrayDataFromDGPort(otherPort, slice, otherSlice);
  FECS_CATCH(false);
}

bool FECS_DGPort_copyAllSlicesDataFromPort(FECS_DGPortRef ref, FECS_DGPortRef otherRef, bool resizeTarget)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTR(DGPortImplPtr, port, false)
  DGPortImplPtr * otherPtr = (DGPortImplPtr *)otherRef;
  if(!otherPtr)
  {
    LoggingImpl::logError("Other port ref is NULL!");
    return false;
  }
  DGPortImplPtr & otherPort = *otherPtr;
  return port->copyAllSlicesDataFromDGPort(otherPort, resizeTarget);
  FECS_CATCH(false);
}

void FECS_DGPort_setOption(FECS_DGPortRef ref, const char * name, const FabricCore::Variant & value)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  return port->setOption(name, value);
  FECS_CATCH_VOID
}

void FECS_DGPort_getOption(FECS_DGPortRef ref, const char * name, FabricCore::Variant & result)
{
  FECS_TRY_CLEARERROR
  GETSMARTPTRVOID(DGPortImplPtr, port)
  result = port->getOption(name);
  FECS_CATCH_VOID
}

// bool FECS_DGPort_isManipulatable(FECS_DGPortRef ref)
// {
//   FECS_TRY_CLEARERROR
//   GETSMARTPTR(DGPortImplPtr, port, false)
//   return port->isManipulatable();
//   FECS_CATCH(false);
// }

// void FECS_DGPort_getAnimationChannels(FECS_DGPortRef ref, FabricCore::RTVal & result)
// {
//   FECS_TRY_CLEARERROR
//   GETSMARTPTRVOID(DGPortImplPtr, port)
//   result = port->getAnimationChannels();  
//   FECS_CATCH_VOID
// }

// void FECS_DGPort_setAnimationChannelValues(FECS_DGPortRef ref, unsigned int nbChannels, float * values)
// {
//   FECS_TRY_CLEARERROR
//   GETSMARTPTRVOID(DGPortImplPtr, port)
//   port->setAnimationChannelValues(nbChannels, values);
//   FECS_CATCH_VOID
// }

// int FECS_DGPort_manipulate(FECS_DGPortRef ref, FabricCore::RTVal & manipulationContext)
// {
//   FECS_TRY_CLEARERROR
//   GETSMARTPTR(DGPortImplPtr, port, 0) // 0 == Manipulation_Event_None
//   return port->manipulate(manipulationContext);
//   FECS_CATCH(0);
// }

// Needed to free memory allocated within the Splice DLL on Windows
void FECS_Free(void *ptr)
{
  free(ptr);
}
