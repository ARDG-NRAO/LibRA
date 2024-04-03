template<typename val_type>
class EnumPar {
public:
  EnumPar(){};
  EnumPar& operator () ( int id, std::string str, std::string desc ){ id_=id; str_=str; desc_=desc; val_=0; return *this; }
  EnumPar& operator () ( int id, std::string str, std::string desc, val_type* val ){ id_=id; str_=str; desc_=desc; val_=val; return *this; }
  int      id()  { return id_;   }
  std::string   str() { return str_; }
  std::string   desc(){ return desc_; }
  val_type val() { return *val_; }
 
private:
  int       id_;
  std::string    str_;
  std::string    desc_;
  val_type* val_;
};
