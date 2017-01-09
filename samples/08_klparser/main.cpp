// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>
#include <vector>

using namespace FabricSplice;

void registerParser(const std::string & owner, const std::string & name, const std::vector<std::string> & lines)
{
  std::string code = lines[0];
  for(size_t i=1;i<lines.size();i++)
    code += "\n"+lines[i];
  KLParser::getParser(owner.c_str(), name.c_str(), code.c_str());
}

#define kl(line) klCode.push_back(line)

int main( int argc, const char* argv[] )
{
  std::vector<std::string> klCode;
  kl("struct MyType {");
  kl("  Float32 x;");
  kl("  Float32 y;");
  kl("}");
  kl("");
  kl("function MyType(Float64 x, Float64 y) {");
  kl("  this.x = x;");
  kl("  this.y = y;");
  kl("}");
  registerParser("MyType", "MyType", klCode);

  klCode.clear();
  kl("require MyType;");
  kl("");
  kl("function Scalar MyType.component(Index index) {");
  kl("  if(index == 0)");
  kl("    return this.x;");
  kl("  return this.y");
  kl("}");
  kl("");
  kl("function Scalar MyType.length() {");
  kl(" return sqrt(this.x * this.x + this.y * this.y);");
  kl("}");
  kl("");
  kl("function Scalar MyType.unit!() {");
  kl("  Scalar l = this.length();");
  kl("  if(l > 0.0) {");
  kl("    this.x /= l;");
  kl("    this.y /= l;");
  kl("  }");
  kl("  return l;");
  kl("}");
  kl("");
  kl("function dumpMyType(MyType m) {");
  kl("  report('x is '+m.x);");
  kl("  report('y is '+m.y);");
  kl("}");
  registerParser("MyType", "MyType_methods", klCode);

  klCode.clear();
  kl("require MyType;");
  kl("const Integer myConst = 12;");
  kl("");
  kl("operator reportMyTypeTask<<<index>>>(io MyType m) {");
  kl("  Scalar len = m.length();");
  kl("  dumpMyType(m);");
  kl("}");
  kl("");
  kl("operator reportMyTypeOp(io MyType m) {");
  kl("  reportMyTypeTask<<<1>>>(m);");
  kl("}");
  registerParser("reportMyTypeOp", "reportMyTypeOp", klCode);

  for(unsigned int i=0;i<KLParser::getNbParsers();i++)
  {
    KLParser parser = KLParser::getParser(i);
    printf("===========================================\n");
    printf("KLParser: '%s' '%s'\n", parser.owner(), parser.name());
    printf("---------------- CODE ---------------------\n");
    printf("%s\n", parser.code());
    printf("--------------- SYMBOLS -------------------\n");
    for(unsigned int j=0;j<parser.getNbKLSymbols();j++)
    {
      KLParser::KLSymbol s = parser.getKLSymbol(j);
      printf("%03d: '%s' '%s'\n", (int)i, s.typeName(), s.str().c_str());
    }
    if(parser.getNbKLRequires() > 0)
    {
      printf("-------------- REQUIRES ------------------\n");
      for(unsigned int j=0;j<parser.getNbKLRequires();j++)
        printf("require: '%s'\n", parser.getKLRequire(j));
    }

    if(parser.getNbKLConstants() > 0)
    {
      printf("-------------- CONSTANTS ------------------\n");
      for(unsigned int j=0;j<parser.getNbKLConstants();j++)
      {
        KLParser::KLConstant c = parser.getKLConstant(j);
        printf("constant '%s' '%s' = '%s'\n", c.type(), c.name(), c.value());
      }
    }
    if(parser.getNbKLStructs() > 0)
    {
      printf("---------- STRUCTS / OBJECTS --------------\n");
      for(unsigned int j=0;j<parser.getNbKLStructs();j++)
      {
        KLParser::KLStruct c = parser.getKLStruct(j);
        printf("%s '%s', %d members.\n", c.type(), c.name(), (int)c.nbMembers());
        for(unsigned k=0;k<c.nbMembers();k++)
          printf("member '%s' '%s'\n", c.memberType(k), c.memberName(k));
      }
    }
    if(parser.getNbKLFunctions() > 0)
    {
      printf("---------- FUNCTIONS / METHODS ------------\n");
      for(unsigned int j=0;j<parser.getNbKLFunctions();j++)
      {
        KLParser::KLFunction c = parser.getKLFunction(j);
        KLParser::KLArgumentList args = c.arguments();
        if(strlen(c.owner()) > 0)
          printf("function '%s', '%s'.'%s', %d arguments.\n", c.type(), c.owner(), c.name(), (int)args.nbArgs());
        else
          printf("function '%s', '%s', %d arguments.\n", c.type(), c.name(), (int)args.nbArgs());

        for(unsigned k=0;k<args.nbArgs();k++)
          printf("argument '%s' %s' '%s'\n", args.mode(k), args.type(k), args.name(k));
      }
    }    
    if(parser.getNbKLOperators() > 0)
    {
      printf("-------------- OPERATORS ------------------\n");
      for(unsigned int j=0;j<parser.getNbKLOperators();j++)
      {
        KLParser::KLOperator c = parser.getKLOperator(j);
        KLParser::KLArgumentList args = c.arguments();
        printf("operator '%s', %d arguments.\n", c.name(), (int)args.nbArgs());
        if(c.isPex())
          printf("pex argument '%s'\n", c.pexArgument());

        for(unsigned k=0;k<args.nbArgs();k++)
          printf("argument '%s' %s' '%s'\n", args.mode(k), args.type(k), args.name(k));
      }
    }
    bool printedHeader = false;
    for(unsigned int j=0;j<parser.getNbKLSymbols();j++)
    {
      KLParser::KLSymbol s = parser.getKLSymbol(j);
      if(s.type() == KLParser::KLSymbol::Type_name || s.type() == KLParser::KLSymbol::Type_this)
      {
        const char * t = parser.getKLTypeForSymbol(s);
        if(strlen(t) == 0)
          continue;
        if(!printedHeader)
        {
          printf("-------------- VARIABLES ------------------\n");
          printedHeader = true;          
        }
        printf("char %d: '%s' of type '%s'\n", (int)s.pos(), s.str().c_str(), t);
      }
    }
    printf("\n");
  }
  return 0;
}
