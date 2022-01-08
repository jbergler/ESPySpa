#ifndef SPANETCONTROLLER_H
#define SPANETCONTROLLER_H

#include <Arduino.h>
#include <RemoteDebug.h>
#include <list>



#define UPDATEFREQUENCY 60000

extern RemoteDebug Debug;

class Register {
    public:
        Register(int req);

        bool updateRegister(const char update[]);
        bool isValid();
        char* getField(int field);

    private:
        int requiredFields;
        char reg[256];
        char* element[64];
        bool valid=false;


};



class Pump {
    public:
    #define PUMP_MODES_COUNT 5
        static const char *pump_modes[PUMP_MODES_COUNT];  // Off = 0, On = 1, Auto = 4

        void initialise(bool installed, bool autoOperation);
        bool isInstalled();
        bool isAutoModeSupported();

        void setOperatingMode(int mode);
        void setOperatingMode(const char *mode);
        int getOperatingMode();
        
    private:
        bool _installed;
        bool _autoOperation;
        int _mode;
};


class SpaNetController {
    public:
        class Light{
            public:
                bool isOn();
                void setIsOn(bool state);
                friend class SpaNetController;

                Light(SpaNetController* p);
                ~Light();

            private:
                SpaNetController* _parent;
                bool _isOn = false;
                void (SpaNetController::*_queueCommand)(String) = nullptr;
        };

        Light lights;

        enum heat_pump_modes {automatic=0, heat=1, cool=2, off=3};
        
        float   getAmps();
        int     getVolts();
        float   getHpumpAmbTemp();
        float   getHpumpConTemp();
        float   getWaterTemp();
        bool    isHeatingOn();
        bool    isUVOn();
        bool    isSanatiseRunning();
        String     getSerialNo();
        char *getStatus();
        float getHeaterTemp();
        
        bool pumpInstalled(int pump);
        Pump *getPump(int pump);

        float   getWaterTempSetPoint();
        bool    setWaterTempSetPoint(float temp);

        heat_pump_modes getHeatPumpMode();
        bool setHeatPumpMode(heat_pump_modes mode);

        bool isAuxHeatingEnabled();
        bool setAuxHeatingEnabled(bool enabled);

        bool setPumpOperating(int pump, int mode);
        void setPumpOperating(int pump, const char *mode);
        bool setPump1Operating(int mode);
        bool setPump2Operating(int mode);
        bool setPump3Operating(int mode);
        bool setPump4Operating(int mode);
        bool setPump5Operating(int mode);

        void queueCommand(String command);

        SpaNetController();
        ~SpaNetController();

        void tick();

        void subscribeUpdate(void (*u)(SpaNetController *));

        void forceUpdate();

        bool initialised();

        Pump pumps[5];

    private:
        bool _firstrun = false;

        float amps; 
        int volts;
        float hpump_amb_temperature;
        float hpump_con_temperature;
        float heater_temperature;

        float waterTemperature;
        float waterTemperatureSetPoint;
        heat_pump_modes heatPumpMode;
        bool auxHeatElement;
        bool heatingActive;
        bool uvActive;
        bool sanatiseActive;
        char* status;

        String serialNo;

        Register registers[13]={1,33,30,31,29,31,34,15,15,15,17,33,18};

        /**
         * @brief time (mils) of next read of spa registers
         * 
         */
        ulong _nextUpdate=millis();

        /**
         * @brief time (mils) of last command queue check/publish
         * 
         */
        ulong lastCommand = millis();

        void (*update)(SpaNetController *) = NULL;

        bool parseStatus(String str);

        /**
         * @brief Sends command to spa controller. Use with care! Preferable use queueCommand to que commands for batch processing.
         * 
         * @param cmd 
         * @return String 
         * 
         */
        String sendCommand(String cmd);

        bool pollStatus();

        std::list<String> commands;
        
        void processCommands();
        void getRegisters();
};


#endif