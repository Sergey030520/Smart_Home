#include <iostream>
#include <sstream>

enum automatic_machine{
    AUTOMATIC_1,//отключение и включение питания всего дома
    AUTOMATIC_2,//отключение и включение основные розетки дома
    AUTOMATIC_3,//отключение и включение свет внутри дома
    AUTOMATIC_4,//отключение и включение наружный свет в саду
    AUTOMATIC_5,//отключение и включение отопление в помещении
    AUTOMATIC_6,//отключение и включение отопление водопровода идущего из скважины с насосом
    AUTOMATIC_7 //отключение и включение кондиционер в помещении
};
enum power_home{
    POWER_OFF,
    POWER_ON
};
enum status_light{
    LIGHT_OFF,
    LIGHT_ON
};
enum heating{
    HEATING_OFF,
    HEATING_ON
};
enum color_temperature_light{
    MIN_LIGHT = 2700,
    MAX_LIGHT = 5000
};
enum status_condition{
    CONDITION_OFF,
    CONDITION_ON
};
enum Temperature_Room{
    LOWER_BAR_ROOM = 22,
    NORMAL_BAR_ROOM = 25,
    TOP_BAR_ROOM = 30
};
enum Temperature_Outside{
    LOWER_BAR_OUTSIDE = 0,
    NORMAL_BAR_OUTSIDE = 5
};

int const count_sensor = 7;
bool automatic_sensors[count_sensor], motion_sensor;
int color_light;


void ShowEventAutomaticLightHouse(){
    std::cout << "Event: the light switch" << (automatic_sensors[3] ? "" : "-off")
        << " sensor in the house was triggered" <<  "!" << std::endl;
}
void ShowEventAutomaticLightGarden(){
    std::cout << "Event: turning " << (automatic_sensors[4] ? "on" : "off")
              << " the lights in the garden" << "!" << std::endl;
}
void ShowEventAutomaticHeatingHouse(){
    std::cout << "Event: the heating in the room was turned " <<
              (automatic_sensors[AUTOMATIC_5] ? "on" : "off") << "!" << std::endl;
}
void ShowEventAutomaticHeatingWaterSupply(){
    std::cout << "Event: the heating of the water supply system was turned " <<
              (automatic_sensors[AUTOMATIC_6] ? "on" : "off") << "!" <<std::endl;
}
void ShowEventAutomaticConditioner(){
    std::cout << "Event: the air conditioner turned " <<
              (automatic_sensors[AUTOMATIC_7] ? "on" : "off") << "!" <<std::endl;
}
void ShowEventMotionSensor(){
    std::cout << "Event: Motion sensor triggered!" << std::endl;
}
void ShowColorTemperature(){
    std::cout << "Color temperature in the house: " << color_light <<std::endl;
}

void TemperatureControlOutside(int temp){
    if (!automatic_sensors[AUTOMATIC_6] and temp < LOWER_BAR_OUTSIDE){
        automatic_sensors[AUTOMATIC_6] = HEATING_ON;
        ShowEventAutomaticHeatingWaterSupply();
    }
    else if(automatic_sensors[AUTOMATIC_6] and temp > NORMAL_BAR_OUTSIDE){
        automatic_sensors[AUTOMATIC_6] = HEATING_OFF;
        ShowEventAutomaticHeatingWaterSupply();
    }
}
void TemperatureControlRoom(int temp){
    if (temp < LOWER_BAR_ROOM and !automatic_sensors[AUTOMATIC_5]){
        automatic_sensors[AUTOMATIC_5] = HEATING_ON;
        ShowEventAutomaticHeatingHouse();
    }
    else if(temp >= NORMAL_BAR_ROOM){
        automatic_sensors[AUTOMATIC_5] = HEATING_OFF;
        if (temp <= TOP_BAR_ROOM and temp > NORMAL_BAR_ROOM and !automatic_sensors[AUTOMATIC_7]){
            automatic_sensors[AUTOMATIC_7] = CONDITION_ON;
            ShowEventAutomaticConditioner();
        }
        ShowEventAutomaticHeatingHouse();
    }
    else if (temp == NORMAL_BAR_ROOM and automatic_sensors[AUTOMATIC_7]){
        automatic_sensors[AUTOMATIC_7] = CONDITION_OFF;
        ShowEventAutomaticConditioner();
    }
}
void GardenLighting(int time){
    if ((time > 16 and time < 24) or (time >= 0 and time < 5)){
        if (!automatic_sensors[AUTOMATIC_4] and motion_sensor){
            automatic_sensors[AUTOMATIC_4] = LIGHT_ON;
            ShowEventAutomaticLightGarden();
        }
        else if (automatic_sensors[AUTOMATIC_4] and !motion_sensor){
            automatic_sensors[AUTOMATIC_4] = LIGHT_OFF;
            ShowEventAutomaticLightGarden();
        }
    }
    else if (automatic_sensors[AUTOMATIC_4]){
        automatic_sensors[AUTOMATIC_4] = LIGHT_OFF;
        ShowEventAutomaticLightGarden();
    }
}
void SmartLight(int time){
    if (time == 0){
        color_light = MAX_LIGHT;
    }
    else if((time >= 16 and time < 20) and (color_light > MIN_LIGHT)){
        color_light -= 575;
    }
}
void HouseLighting(bool status, int time){
    if (status and !automatic_sensors[AUTOMATIC_3]){
        automatic_sensors[AUTOMATIC_3] = LIGHT_ON;
        ShowEventAutomaticLightHouse();
    }
    else if(!status and automatic_sensors[AUTOMATIC_3]){
        automatic_sensors[AUTOMATIC_3] = LIGHT_OFF;
        ShowEventAutomaticLightHouse();
    }
    SmartLight(time);
    if(automatic_sensors[AUTOMATIC_3]) ShowColorTemperature();
}
void MotionSensor(bool status){
    if (motion_sensor != status){
        motion_sensor = status;
        if (status) ShowEventMotionSensor();
    }
}

void SimulationSmartHome(int days) {
    automatic_sensors[AUTOMATIC_1] = POWER_ON;
    for (int day = 0; day < days; ++day) {
        for (int hour = 0; hour < 24 and automatic_sensors[AUTOMATIC_1]; ++hour) {
            std::stringstream date;
            int date_temp[2];//0 - outside temperature, 1 - room temperature
            std::string status_movement, status_light;
            std::cout << "Input temperature outside, temperature inside, is there any movement outside, "
                         "is the light on in the house(day: " << day << " hour: " << hour << "): " << std::endl;
            std::string copy_date;
            std::getline(std::cin, copy_date);
            date << copy_date;
            date >> date_temp[0] >> date_temp[1] >> status_movement >> status_light;
            std::cout << date.str() << std::endl;
            TemperatureControlOutside(date_temp[0]);
            TemperatureControlRoom(date_temp[1]);
            MotionSensor((status_movement == "yes" ? 1 : 0));
            GardenLighting(hour);
            HouseLighting(((status_light == "on") ? 1 : 0), hour);
        }
    }
    automatic_sensors[AUTOMATIC_1] = POWER_OFF;
    for (int automatic_sensor = 1; automatic_sensor < count_sensor; ++automatic_sensor)
        automatic_sensors[automatic_sensor] = POWER_OFF;
}
int main() {
    SimulationSmartHome(2);
}
