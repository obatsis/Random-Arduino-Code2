enum {aws_t, tws_t, awa_t, twa_t, twd_t, 
  cog_t, sog_t, odo_t, 
  mta_t, mtw_t, 
  dbt_t, dbs_t, 
  len_t};

const char name[len_t][4]= {"AWS","TWS","AWA","TWA","TWD",
  "COG","SOG","ODO",
  "MTA","MTW",
  "DBT","DBS"};

const uint8_t intens[5] = {20, 50, 100, 150, 255}; //max 255
const int text_intens[5] = {1, 4, 8, 12, 15}; //max 15

class SailData {
public:
  SailData() {
    for(int i=0;i<len_t;i++)  {
      data[i] = 888;
    }
  }
  float data[len_t];
    
  print_all(HardwareSerial* stream){
    for(int i=0;i<len_t;i++)  {
      stream->print(name[i]);
      stream->print(": ");
      stream->print(data[i]);
      stream->print('\t');
      stream->println();  
    }
  }
};
