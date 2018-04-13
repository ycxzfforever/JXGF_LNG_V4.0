#ifndef  __CRC_H
#define  __CRC_H
#define CRC_POLY_CCITT ((uint8_t)0x00)   /* CRC32 */
#define CRC_POLY_16    ((uint8_t)0x01)   /* CRC16 */
#define CRC_POLY_32    ((uint8_t)0x02)   /* CRC-CCITT */

/*@brief CRC_bit_order definitions 
  * @{
  */
#define CRC_BIT_RVS_WR_NO    ((uint8_t)0x00)  
#define CRC_BIT_RVS_WR_EN    ((uint8_t)0x04)  

/*@brief CRC_data_complement definitions 
  * @{
  */
#define CRC_CMPL_WR_NO    ((uint8_t)0x00)  
#define CRC_CMPL_WR_EN    ((uint8_t)0x08)  

/*@brief CRC_bit_sum definitions 
  * @{
  */
#define CRC_BIT_RVS_SUM_NO    ((uint8_t)0x00)   
#define CRC_BIT_RVS_SUM_EN    ((uint8_t)0x10)  

/*@brief CRC_sum_complement definitions 
  * @{
  */
#define CRC_CMPL_SUM_NO    ((uint8_t)0x00)   
#define CRC_CMPL_SUM_EN    ((uint8_t)0x20)  

/* Public Types --------------------------------------------------------------- */
typedef struct {
    uint8_t     poly;       /* !< CRC polynomial  
                                                 This parameter can be a value of @ref CRC_polynomial_type */
    uint8_t     bit_wr;     /* !< CRC bit order reverse   
                                                 This parameter can be a value of @ref CRC_bit_order */
    uint8_t     cmpl_wr;    /* !< CRC data complement   
                                                 This parameter can be a value of @ref CRC_data_complement */
    uint8_t     bit_sum;    /* !< CRC bit sum   
                                                 This parameter can be a value of @ref CRC_bit_sum */
    uint8_t     cmpl_sum;   /* !< CRC sum complement   
                                                 This parameter can be a value of @ref CRC_sum_complement */ 
    uint32_t    seed;       /* !< seed value */
} CRC_CFG_Type;

uint16_t CRC16Calculate(uint8_t *data, uint32_t len);

#endif
