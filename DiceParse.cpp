#include <algorithm>
#include <any>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <deque>
#include <exception>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>
#include <variant>

#define usestod // Use this option if your compiler does NOT allow non-integer transform ... like me (due to this probably modify required when you disable)

//#define Debug // function will output some results(recommend to edit further)

namespace DiceParser{

    std::random_device SeedGen;
    std::default_random_engine Engine(SeedGen());

    typedef char16_t _char;
    typedef std::variant<double,_char,std::string> _Variant;

    const _char 
                LeftBracket = u'(' , RightBracket = u')' ,
                Add = u'+' , Sub = u'-' , Mul = u'*' , Div = u'/' , Roll = u'd' , Comma = u',' ,
                Commentout = u';' , Period = u'.' , Func = u'f';

    const std::unordered_map<_char,unsigned> OperatorPrecedence{
        {LeftBracket,0},
        {RightBracket,0},
        {Func,0},
        {Roll,3},
        {Mul,2},
        {Div,2},
        {Add,1},
        {Sub,1},
    };

    // actually this is wrapper of std
    const std::unordered_map<std::string,std::tuple<std::string,unsigned,std::any >> Functions{
        {"pi",std::make_tuple("pi",0u,  (std::function<double(void)>)[]()->double{return 3.14159265;})},
        {"tau",std::make_tuple("tau",0u,  (std::function<double(void)>)[]()->double{return 3.14159265*2.;})},

        {"floor",std::make_tuple("floor",1u,(std::function<double(double)>)[](double x)->double{return static_cast<double>(std::floor(x));})},
        {"ceil",std::make_tuple("ceil",1u,  (std::function<double(double)>)[](double x)->double{return static_cast<double>(std::ceil(x) ); })},
        {"sin",std::make_tuple("sin",1u,  (std::function<double(double)>)[](double x)->double{return static_cast<double>(std::sin(x) ); })},
        {"cos",std::make_tuple("cos",1u,  (std::function<double(double)>)[](double x)->double{return static_cast<double>(std::cos(x) ); })},
        {"tan",std::make_tuple("tan",1u,  (std::function<double(double)>)[](double x)->double{return static_cast<double>(std::tan(x) ); })},
        {"sqrt",std::make_tuple("sqrt",1u,  (std::function<double(double)>)[](double x)->double{return static_cast<double>(std::sqrt(x) ); })},

        {"pow",std::make_tuple("pow",2u,  (std::function<double(double,double)>)[](double x,double y)->double{return static_cast<double>(std::pow(x,y) ); })},
    };
    
    double parse(std::basic_string<_char> Target){

        std::deque<_Variant> OutArr,AuxArr;
        std::vector<_Variant> OpArr;
        std::string TempBuffer;

        bool isNumber = false, Alert = false,isFunction = false;
        auto f = [&OutArr,&TempBuffer,&isNumber](bool Reset=true){ // Lambda function for compacting codes
            #ifdef usestod
                OutArr.push_back(std::stod(TempBuffer.c_str()));
            #else
                int FromCharsTemp = 0;
                std::from_chars(TempBuffer.c_str(),TempBuffer.c_str()+TempBuffer.length(),FromCharsTemp);OutArr.push_back(FromCharsTemp);TempBuffer.clear();
            #endif
            TempBuffer.clear();if(Reset)isNumber = false;
        };

        _char last_val;

        for(int i=0;i<Target.size();i++){
            _char val = Target[i];

            if(std::isdigit(val) || val==Period || ((val==Sub || val==Add) && last_val!=RightBracket && !isNumber) && !isFunction ){
                isNumber = true;
                TempBuffer += val;
            }else{
                if(val == Commentout) break;
                else if(val == u' ')continue;

                if(isNumber) f(0);
                val = std::tolower(val);
                if(!isalpha(val)){
                    _char OpBack = ' ';
                    if(!OpArr.empty()){
                        if(std::holds_alternative<_char>(OpArr.back()))
                            OpBack = std::get<_char>(OpArr.back());
                        else
                            OpBack = Func;
                    }
                    switch(val){
                        case LeftBracket:{
                            if(isFunction){
                                isFunction = false;
                                if(Functions.contains(TempBuffer))
                                    OpArr.push_back( std::get<0>(Functions.at(TempBuffer)) );
                                TempBuffer.clear();
                            }
                            if((isNumber&&(Alert=true)) || last_val == RightBracket )OpArr.push_back(Mul);OpArr.push_back(val);
                        }break;
                        case RightBracket:
                            {
                                while( !OpArr.empty() && OpBack != LeftBracket){OutArr.push_back(OpBack);OpArr.pop_back();
                                    if(!OpArr.empty()){
                                        if(std::holds_alternative<_char>(OpArr.back()))
                                            OpBack = std::get<_char>(OpArr.back());
                                        else
                                            OpBack = 0;
                                    }
                                };
                                if(OpArr.empty())throw std::runtime_error("Mismatch brackets");OpArr.pop_back();
                                if(!OpArr.empty())
                                    if(!std::holds_alternative<_char>(OpArr.back()))
                                        {OutArr.push_back(OpArr.back());OpArr.pop_back();}
                            }break;
                        case Comma:
                            while( !OpArr.empty() && OpBack != LeftBracket){OutArr.push_back(OpBack);OpArr.pop_back();}break;
                        default:
                            if( !OperatorPrecedence.contains(val) )throw std::runtime_error("Unknown operator");
                            unsigned currentprec = OperatorPrecedence.at(val);
                            _char op2;
                            while( !OpArr.empty() && OperatorPrecedence.contains(op2=OpBack) && OperatorPrecedence.at(op2)>=currentprec){
                                OutArr.push_back(op2);OpArr.pop_back();
                                if(!OpArr.empty()){
                                    if(std::holds_alternative<_char>(OpArr.back()))
                                        OpBack = std::get<_char>(OpArr.back());
                                    else
                                        OpBack = 0;
                                }
                            }
                            OpArr.push_back(val);break;
                    }
                }else{
                    isFunction = true;
                    TempBuffer += val;
                }
                isNumber = false;
            }
            last_val = val;
        }
        if(!TempBuffer.empty()) f();

        while( !OpArr.empty() ){ OutArr.push_back(OpArr.back());OpArr.pop_back();}
        #ifdef Debug
            for(auto i:OutArr){
                if(std::holds_alternative<_char>(i))
                    std::printf("%c ",std::get<_char>(i));
                else if(std::holds_alternative<double>(i))
                    std::cout<<std::get<0>(i)<<" ";
                else
                    std::cout<<"[F]"<<" ";
                
            }
            std::cout<<" << END"<<std::endl;
        #endif

        AuxArr.swap(OutArr);
        unsigned stopcount=0;

        auto PopValue = [&OutArr](void)->double{
            if(!OutArr.empty()){
                double x = std::get<double>(OutArr.back());OutArr.pop_back();
                return x;
            }else throw std::runtime_error("Not enough operand");
        };

        while(!AuxArr.empty() && stopcount++<1024){
            _Variant _val = AuxArr.front();
            if(std::holds_alternative<double>(_val)){
                OutArr.push_back(std::get<double>(_val));
            }else if(std::holds_alternative<_char>(_val)){
                _char val = std::get<_char>(_val);
                double v1,v2;
                if(OutArr.size()>1){
                    switch(val){
                        case Add:v1=PopValue();v2=PopValue();OutArr.push_back(v2+v1);break;
                        case Sub:v1=PopValue();v2=PopValue();OutArr.push_back(v2-v1);break;
                        case Mul:v1=PopValue();v2=PopValue();OutArr.push_back(v2*v1);break;
                        case Div:v1=PopValue();v2=PopValue();OutArr.push_back(v2/v1);break;
                        case Roll:{
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
                // This is awful. Probably technical debt
                auto Data = Functions.at(std::get<std::string>(_val));
                unsigned augc = std::get<1>(Data);
                switch(augc){
                    case 0u:{
                        auto Func = std::any_cast<std::function<double()>>(std::get<2>(Data));
                        OutArr.push_back( Func() );}break;
                    case 1u:{
                        auto Func = std::any_cast<std::function<double(double)>>(std::get<2>(Data));
                        OutArr.push_back( Func( PopValue() ) );}break;
                    case 2u:{
                        auto Func = std::any_cast<std::function<double(double,double)>>(std::get<2>(Data));
                        OutArr.push_back( Func( PopValue(), PopValue() ) );}break;
                    default:throw std::runtime_error("Too many augurments");
                }
            }
            AuxArr.pop_front();
        }
        #ifdef Debug
            std::cout<<" [ ";
            for(auto i:OutArr){
                if(std::holds_alternative<_char>(i)){
                    std::printf("%c ",std::get<_char>(i));
                }else{
                    std::cout<<std::get<0>(i)<<" ";
                }
            }std::cout<<"] "<<std::endl;
        #endif
        //std::cout<<std::endl;
        //if(Alert)std::cout<<"The formula may be ambiguous!"<<std::endl; // Alert : become true when detected something like : 2(3+4)
        if( OutArr.size()<2 )
            if(!OutArr.empty())
                return std::get<double>(OutArr.back());
            else
                return 0.;
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
            std::getline(std::cin,IN);
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
