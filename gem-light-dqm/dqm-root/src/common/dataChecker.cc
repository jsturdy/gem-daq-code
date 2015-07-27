class dataChecker {
    private:
        uint16_t crc_calc(uint16_t crc_in, uint16_t dato)
        {
            uint16_t v = 0x0001;
            uint16_t mask = 0x0001;    
            bool d=0;
            uint16_t crc_temp = crc_in;
            unsigned char datalen = 16;
             
            for (int i=0; i<datalen; i++){
                if (dato & v) d = 1;
                else d = 0;
                if ((crc_temp & mask)^d) crc_temp = crc_temp>>1 ^ 0x8408;
                else crc_temp = crc_temp>>1;
                v<<=1;
            }
            return(crc_temp);
        }

    public:
        dataChecker(){}
        ~dataChecker(){}
        // CRC checker
        uint16_t checkCRC(uint16_t dataVFAT[11], bool OKprint)
        {
            uint16_t crc_fin = 0xffff;
            for (int i = 11; i >= 1; i--)
            {
                crc_fin = this->crc_calc(crc_fin, dataVFAT[i]);
        	    /*
        	    if(OKprint){
                      cout << " dataVFAT[" << std::setfill('0') 
                      << std::setw(2) << i << "] " << hex << std::setfill('0') << std::setw(4) << dataVFAT[i]
                           << " crc_temp " << std::setfill('0') << std::setw(4) << crc_fin << dec << endl;
                }
        	    */
            }
            return(crc_fin);
        }

};
