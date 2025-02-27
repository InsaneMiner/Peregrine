#ifndef PEREGRINE_SYM_TABLE_HPP
#define PEREGRINE_SYM_TABLE_HPP

#include "ast/ast.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <algorithm>
#include <string>
#include <optional>

template<typename T>
class SymbolTable {
    std::map<std::string, T> m_symbols;
    std::shared_ptr<SymbolTable<T>> m_parent;

  public:
    SymbolTable(std::shared_ptr<SymbolTable> parent) {
        m_parent = parent;
    }

    std::optional<T> get(std::string name) {
        if (m_symbols.find(name) == m_symbols.end()) {
            if (!m_parent) {
                return std::nullopt; // the symbol was not found
            }

            return m_parent->get(name);
        }
        
        return m_symbols[name];
    }

    bool set(std::string name, T value) {
        if (get(name)) {
            return false; // the symbol has been defined already
        }

        m_symbols[name] = value;
        get(name);
        return true;
    }

    bool reassign(std::string name, T value) {
        if (m_symbols.find(name) == m_symbols.end()) {
            if (!m_parent) {
                return false; // the symbol was not found
            }

            return m_parent->reassign(name, value);
        }

        m_symbols[name] = value;
        return true;
    }

    std::shared_ptr<SymbolTable<T>> parent() {
        return m_parent;
    }
};

class MangleName{
    std::map<std::string, std::string> m_global_names={
                                                        {"error","error"},
                                                        {"printf","printf"},
                                                        {"i8","int8_t"},
                                                        {"i16","int16_t"},
                                                        {"i32","int32_t"},
                                                        {"int","int64_t"},
                                                        {"u8","uint8_t"},
                                                        {"u16","uint16_t"},
                                                        {"u32","uint32_t"},
                                                        {"uint","uint64_t"},
                                                        {"f32","float"},
                                                        {"float","double"},
                                                        };
    std::map<std::string, std::string> m_local_names;
    public:
    MangleName()=default;
    MangleName(MangleName const &other){
        *this=other;
    }
    void set_local(std::string original){
        m_local_names[original]="____P____P____"+original;
    }
    void set_local(std::string original,std::string mangled){
        m_local_names[original]=mangled;
    }
    void set_global(std::string original,std::string mangled){
        m_global_names[original]=mangled;
    }
    
    bool contains(std::string name){
        if(m_local_names.count(name)!=0){
            return true;
        }
        else if(m_global_names.count(name)!=0){
            return true;
        }
        return false;
    }
    std::string operator[](std::string name){
        if(m_local_names.count(name)!=0){
            return m_local_names[name];
        }
        else if(m_global_names.count(name)!=0){
            return m_global_names[name];
        }
        else{
            return name;
        }
    }
    void print(){
        std::cout<<"Local{\n";
        for(auto const &p:m_local_names){
            std::cout<<"    "<<p.first<<":"<<p.second<<std::endl;
        }
        std::cout<<"}\nGLOBAL{\n";
        for(auto const &p:m_global_names){
            std::cout<<"    "<<p.first<<":"<<p.second<<std::endl;
        }
        std::cout<<"}";
    }
};

#endif