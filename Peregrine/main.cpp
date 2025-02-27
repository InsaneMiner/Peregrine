#include "analyzer/typeChecker.hpp"
#include "docgen/html/docgen.hpp"
#include "codegen/cpp/codegen.hpp"
#include "analyzer/ast_validate.hpp"
#include "cli/cli.hpp"
#include "codegen/js/codegen.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokens.hpp"
#include "parser/parser.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>


bool check_file_existence(std::string fname){
    FILE *file; 

    file = fopen( fname.c_str() , "r");

   if (file != NULL) {
      fclose(file);
      return true;
   } else {
      return false;
   }
}


void compile(cli::state s){
    if (s.dev_debug){
        std::ifstream file("../Peregrine/test.pe");
        std::stringstream buf;
        buf << file.rdbuf();

        std::vector<Token> tokens = lexer(buf.str(), "test");

        for (auto& token : tokens) {
            std::cout << "Keyword= " << token.keyword
                      << " Type= " << token.tkType <<" Line= "<<token.line<<" Loc="<<token.location<<"\n";
        }
        Parser parser(tokens, "test");
        ast::AstNodePtr program = parser.parse();
        std::cout << program->stringify() << "\n";
        // astValidator::Validator val(program,"test");
        TypeChecker typeChecker(program);
    }
    else{

        if (check_file_existence(s.input_filename)){
            std::ifstream file(s.input_filename);
            std::stringstream buf;
            buf << file.rdbuf();
            auto filename=s.input_filename;
            std::string path = realpath(filename.c_str(), NULL);
            std::vector<Token> tokens = lexer(buf.str(), path);
            Parser parser(tokens,path);
            ast::AstNodePtr program = parser.parse();
            astValidator::Validator val(program,filename);
            auto output=s.output_filename;
            
            if (s.emit_js){
                js::Codegen codegen(output, program, false, path);
            }else if(s.emit_html){
                js::Codegen codegen(output, program, true, path);
            }else if(s.doc_html){
                html::Docgen Docgen(output, program, path);
            }else if(s.emit_cpp){
                cpp::Codegen codegen(output, program,path);
            }else if(s.emit_obj){
                cpp::Codegen codegen("temp.cc", program,path);
                auto cmd=s.cpp_compiler+" -fno-exceptions -c -std=c++20 temp.cc -fpermissive -w "+s.cpp_arg+" -o "+output;
                system(cmd.c_str());
                system("rm temp.cc");
            }else{
                cpp::Codegen codegen("temp.cc", program,path);
                auto cmd=s.cpp_compiler+" -fno-exceptions -std=c++2a temp.cc -fpermissive -w "+s.cpp_arg+" -o "+output;
                system(cmd.c_str());
                system("rm temp.cc");
            }
        }
        else{
            std::cout << "error: file with name of \"" << s.input_filename << "\" does not exist\n";
        }
        
    }
}
int main(int argc, char** argv) {
    cli::CLI cli(argc, argv);
    cli::state state = cli.parse();
    if (argc== 1) {
        cli::help();
        return 0;
    } else {
        state.validate_state();
        compile(state);
    }
    return 0;
}
