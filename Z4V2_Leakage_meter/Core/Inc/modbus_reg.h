/// @file    modbus_reg.h
/// @brief   Modbus register access function   header
/// @author  Y.Sugawara
/// @date    2026/4/28
/// @version 1.0


#ifndef INC_MODBUS_REG_H_
#define INC_MODBUS_REG_H_

int MODBUS_set_reg(uint16_t add, int16_t data) ;
int MODBUS_get_reg(uint16_t add, int16_t *val) ;


#endif /* INC_MODBUS_REG_H_ */
