
#ifndef CASAC_QUANTITY_H
#define CASAC_QUANTITY_H 1

namespace casac {
   struct Quantity
   {
      Quantity( ) { }
      Quantity(std::vector<double> arg0, std::string arg1) : value(arg0), units(arg1) { }
      std::vector<double> value;
      std::string units;

   };
}
#endif
