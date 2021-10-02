#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <variant>
#include <deque>
#include <exception>
#include <charconv>
#include <random>
#include <algorithm>
#include <unordered_map>

#define usestod // Use this option if your compiler does NOT allow non-integer transform ... like me

//#define Debug

// 255 255 255 255 - * ( + )

namespace DiceParser{

    std::random_device SeedGen;
    std::default_random_engine Engine(SeedGen());

    typedef char16_t _char;
    typedef std::variant<double,_char> _Variant;

    const _char 
                LeftBracket = u'(' , RightBracket = u')' ,
                Add = u'+' , Sub = u'-' , Mul = u'*' , Div = u'/' ;

    std::unordered_map<_char,unsigned> OperatorPrecedence{
        {LeftBracket,0},
        {RightBracket,0},
        {u'd',3},
        {Mul,2},
        {Div,2},
        {Add,1},
        {Sub,1},
    };
    
    double parse(std::basic_string<_char> Target){
        //std::basic_string<_char> Target = u"3d3d3d3";
        std::deque<_Variant> OutArr,AuxArr;
        std::vector<_char> OpArr;
        std::string TempNum;

        bool isNumber = false, Alert = false;
        auto f = [&OutArr,&TempNum,&isNumber](bool Reset=true){ // Lambda function for compacting codes
            #ifdef usestod
                OutArr.push_back(std::stod(TempNum.c_str()));
            #else
                int FromCharsTemp = 0;
                std::from_chars(TempNum.c_str(),TempNum.c_str()+TempNum.length(),FromCharsTemp);OutArr.push_back(FromCharsTemp);TempNum.clear();
            #endif
            TempNum.clear();if(Reset)isNumber = false;
        };
        _char last_val;
        for(int i=0;i<Target.size();i++){
            _char val = Target[i];
            if(std::isdigit(val) || val=='.' || ((val==Sub || val==Add) && !isNumber)){
                isNumber = true;
                TempNum += val;
            }else{
                if(isNumber) f(0);
                //OpArr.push_back(val);
                val = std::tolower(val);
                switch(val){
                    case LeftBracket: if((isNumber&&(Alert=true||1)) || last_val == RightBracket )OpArr.push_back(u'*');OpArr.push_back(val);break;
                    case RightBracket:
                        {
                        while( !OpArr.empty() && OpArr.back() != LeftBracket){OutArr.push_back(OpArr.back());OpArr.pop_back();};
                        if(OpArr.empty())throw std::runtime_error("Mismatch brackets");OpArr.pop_back();
                        }
                        break;
                    default:
                        if( !OperatorPrecedence.contains(val) )throw std::runtime_error("Unknown operator");
                        unsigned currentprec = OperatorPrecedence.at(val);
                        _char op2;
                        while( !OpArr.empty() && OperatorPrecedence.at(op2=OpArr.back())>=currentprec){
                            OutArr.push_back(op2);OpArr.pop_back();
                        }
                        OpArr.push_back(val);break;
                }
                isNumber = false;
            }
            last_val = val;
        }
        if(!TempNum.empty()) f();

        while( !OpArr.empty() ){ OutArr.push_back(OpArr.back());OpArr.pop_back();}
        #ifdef Debug
            for(auto i:OutArr){
                if(std::holds_alternative<_char>(i)){
                    std::printf("%c ",std::get<_char>(i));
                }else{
                    std::cout<<std::get<0>(i)<<" ";
                }
            }
        #endif

        AuxArr.swap(OutArr);

        unsigned stopcount=0;
        while(!AuxArr.empty() && stopcount++<1024){
            _Variant _val = AuxArr.front();
            if(std::holds_alternative<_char>(_val)){
                _char val = std::get<_char>(_val);
                double v1,v2;
                if(OutArr.size()>1){
                    switch(val){
                        case Add:v1=std::get<double>(OutArr.back());OutArr.pop_back();v2=std::get<double>(OutArr.back());OutArr.pop_back();OutArr.push_back(v2+v1);break;
                        case Sub:v1=std::get<double>(OutArr.back());OutArr.pop_back();v2=std::get<double>(OutArr.back());OutArr.pop_back();OutArr.push_back(v2-v1);break;
                        case Mul:v1=std::get<double>(OutArr.back());OutArr.pop_back();v2=std::get<double>(OutArr.back());OutArr.pop_back();OutArr.push_back(v2*v1);break;
                        case Div:v1=std::get<double>(OutArr.back());OutArr.pop_back();v2=std::get<double>(OutArr.back());OutArr.pop_back();OutArr.push_back(v2/v1);break;
                        case u'd':{
                            v1=std::min(std::get<double>(OutArr.back()),1024.);OutArr.pop_back();
                            v2=std::get<double>(OutArr.back());OutArr.pop_back();
                            std::uniform_int_distribution<> dist(1l,static_cast<double>(v2));
                            double ans=0.;for(int i=0;i<v1;i++)ans += static_cast<double>(dist(Engine));
                            OutArr.push_back(ans);break;
                            }
                        default:throw std::runtime_error("Invalid operator");break;
                    }
                }
            }else{
                OutArr.push_back(std::get<double>(_val));
            }
            AuxArr.pop_front();
        }
        #ifdef Debug
            std::cout<<" > ";
            for(auto i:OutArr){
                if(std::holds_alternative<_char>(i)){
                    std::printf("%c ",std::get<_char>(i));
                }else{
                    std::cout<<std::get<0>(i)<<" ";
                }
            }std::cout<<std::endl;
        #endif
        //std::cout<<std::endl;
        //if(Alert)std::cout<<"The formula may be ambiguous!"<<std::endl;
        if( OutArr.size()<2 )
            return std::get<double>(OutArr.back());
        else
            throw std::runtime_error("Has multiple output");
    }
}

std::u16string stdstrtou16str(std::string x){
    std::u16string y;
    for(int i=0;i<x.length();i++)
        y+=(char16_t)x[i];
    return y;
}

int main(int argc,char *argv[]){
    try{
        int Value = 0x0000ffff;
        std::cout<<"Start"<<std::endl;
        std::string IN;
        do{
            std::cout<<" > ";
            std::cin>>IN;
            if(IN!="exit")
                try{
                    double x = DiceParser::parse(stdstrtou16str(IN));
                    std::cout<<"Out = "<<x<<std::endl;
                }catch(std::exception &E){
                    std::cout<<"Exception : "<<E.what()<<std::endl;
                }
                
        }while(IN!="exit");
        std::cout<<"exit"<<std::endl;
    }catch(std::exception &E){
        std::cout<<std::endl<<E.what()<<std::endl<<"Something went wrong!"<<std::endl;
    }
    return 0;
}

// standalone(meaningless)
// g++ -fPIC -static -lstdc++ -lgcc -std=c++20 DiceParse.cpp -o DiceParse.exe && ".\DiceParse.exe"

// gcc required
// g++ -std=c++17 DiceParse.cpp -o DiceParse.exe && ".\DiceParse.exe"
