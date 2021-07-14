#include <fsfw_hal/stm32h7/dma.h>
#include "mspInit.h"
#include "spiCore.h"
#include "spiInterrupts.h"
#include "stm32h743xx.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_def.h"

#include <stdio.h>

spi::msp_func_t mspInitFunc = nullptr;
spi::MspCfgBase* mspInitArgs = nullptr;

spi::msp_func_t mspDeinitFunc = nullptr;
spi::MspCfgBase* mspDeinitArgs = nullptr;

/**
 * @brief SPI MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - DMA configuration for transmission request by peripheral
 *           - NVIC configuration for DMA interrupt request enable
 * @param hspi: SPI handle pointer
 * @retval None
 */
void spi::halMspInitDma(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = dynamic_cast<MspDmaConfigStruct*>(cfgBase);
    if(hspi == nullptr or cfg == nullptr) {
        return;
    }
    setSpiHandle(hspi);

    DMA_HandleTypeDef* hdma_tx = nullptr;
    DMA_HandleTypeDef* hdma_rx = nullptr;
    spi::getDmaHandles(&hdma_tx, &hdma_rx);
    if(hdma_tx == nullptr or hdma_rx == nullptr) {
        printf("HAL_SPI_MspInit: Invalid DMA handles. Make sure to call setDmaHandles!\n");
        return;
    }

    spi::halMspInitInterrupt(hspi, cfg);

    // DMA setup
    if(cfg->dmaClkEnableWrapper == nullptr) {
        mspErrorHandler("spi::halMspInitDma", "DMA Clock init invalid");
    }
    cfg->dmaClkEnableWrapper();

    // Configure the DMA
    /* Configure the DMA handler for Transmission process */
    if(hdma_tx->Instance == nullptr) {
        // Assume it was not configured properly
        mspErrorHandler("spi::halMspInitDma", "DMA TX handle invalid");
    }

    HAL_DMA_Init(hdma_tx);
    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, *hdma_tx);

    HAL_DMA_Init(hdma_rx);
    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, *hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
    // Assign the interrupt handler
    dma::assignDmaUserHandler(cfg->rxDmaIndex, cfg->rxDmaStream, &spi::dmaRxIrqHandler, hdma_rx);
    HAL_NVIC_SetPriority(cfg->rxDmaIrqNumber, cfg->rxPreEmptPriority, cfg->rxSubpriority);
    HAL_NVIC_EnableIRQ(cfg->rxDmaIrqNumber);

    /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
    // Assign the interrupt handler
    dma::assignDmaUserHandler(cfg->txDmaIndex, cfg->txDmaStream,
            &spi::dmaTxIrqHandler, hdma_tx);
    HAL_NVIC_SetPriority(cfg->txDmaIrqNumber, cfg->txPreEmptPriority, cfg->txSubpriority);
    HAL_NVIC_EnableIRQ(cfg->txDmaIrqNumber);
}

/**
 * @brief SPI MSP De-Initialization
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO, DMA and NVIC configuration to their default state
 * @param hspi: SPI handle pointer
 * @retval None
 */
void spi::halMspDeinitDma(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = dynamic_cast<MspDmaConfigStruct*>(cfgBase);
    if(hspi == nullptr or cfg == nullptr) {
        return;
    }
    spi::halMspDeinitInterrupt(hspi, cfgBase);
    DMA_HandleTypeDef* hdma_tx = NULL;
    DMA_HandleTypeDef* hdma_rx = NULL;
    spi::getDmaHandles(&hdma_tx, &hdma_rx);
    if(hdma_tx == NULL || hdma_rx == NULL) {
        printf("HAL_SPI_MspInit: Invalid DMA handles. Make sure to call setDmaHandles!\n");
    }
    else {
        // Disable the DMA
        /* De-Initialize the DMA associated to transmission process */
        HAL_DMA_DeInit(hdma_tx);
        /* De-Initialize the DMA associated to reception process */
        HAL_DMA_DeInit(hdma_rx);
    }

    // Disable the NVIC for DMA
    HAL_NVIC_DisableIRQ(cfg->txDmaIrqNumber);
    HAL_NVIC_DisableIRQ(cfg->rxDmaIrqNumber);

}

void spi::halMspInitPolling(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = dynamic_cast<MspPollingConfigStruct*>(cfgBase);
    GPIO_InitTypeDef GPIO_InitStruct = {};
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    cfg->setupMacroWrapper();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = cfg->sckPin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = cfg->sckAlternateFunction;
    HAL_GPIO_Init(cfg->sckPort, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = cfg->misoPin;
    GPIO_InitStruct.Alternate = cfg->misoAlternateFunction;
    HAL_GPIO_Init(cfg->misoPort, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = cfg->mosiPin;
    GPIO_InitStruct.Alternate = cfg->mosiAlternateFunction;
    HAL_GPIO_Init(cfg->mosiPort, &GPIO_InitStruct);
}

void spi::halMspDeinitPolling(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = reinterpret_cast<MspPollingConfigStruct*>(cfgBase);
    // Reset peripherals
    cfg->cleanUpMacroWrapper();

    // Disable peripherals and GPIO Clocks
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(cfg->sckPort, cfg->sckPin);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(cfg->misoPort, cfg->misoPin);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(cfg->mosiPort, cfg->mosiPin);
}

void spi::halMspInitInterrupt(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = dynamic_cast<MspIrqConfigStruct*>(cfgBase);
    if(cfg == nullptr or hspi == nullptr) {
        return;
    }

    spi::halMspInitPolling(hspi, cfg);
    // Configure the NVIC for SPI
    spi::assignSpiUserHandler(cfg->spiBus, cfg->spiIrqHandler, cfg->spiUserArgs);
    HAL_NVIC_SetPriority(cfg->spiIrqNumber, cfg->preEmptPriority, cfg->subpriority);
    HAL_NVIC_EnableIRQ(cfg->spiIrqNumber);
}

void spi::halMspDeinitInterrupt(SPI_HandleTypeDef* hspi, MspCfgBase* cfgBase) {
    auto cfg = dynamic_cast<MspIrqConfigStruct*>(cfgBase);
    spi::halMspDeinitPolling(hspi, cfg);
    // Disable the NVIC for SPI
    HAL_NVIC_DisableIRQ(cfg->spiIrqNumber);
}

void spi::getMspInitFunction(msp_func_t* init_func, MspCfgBase** args) {
    if(init_func != NULL && args != NULL) {
        *init_func = mspInitFunc;
        *args = mspInitArgs;
    }
}

void spi::getMspDeinitFunction(msp_func_t* deinit_func, MspCfgBase** args) {
    if(deinit_func != NULL && args != NULL) {
        *deinit_func = mspDeinitFunc;
        *args = mspDeinitArgs;
    }
}

void spi::setSpiDmaMspFunctions(MspDmaConfigStruct* cfg,
        msp_func_t initFunc, msp_func_t deinitFunc) {
    mspInitFunc = initFunc;
    mspDeinitFunc = deinitFunc;
    mspInitArgs = cfg;
    mspDeinitArgs = cfg;
}

void spi::setSpiIrqMspFunctions(MspIrqConfigStruct *cfg, msp_func_t initFunc,
        msp_func_t deinitFunc) {
    mspInitFunc = initFunc;
    mspDeinitFunc = deinitFunc;
    mspInitArgs = cfg;
    mspDeinitArgs = cfg;
}

void spi::setSpiPollingMspFunctions(MspPollingConfigStruct *cfg, msp_func_t initFunc,
        msp_func_t deinitFunc) {
    mspInitFunc = initFunc;
    mspDeinitFunc = deinitFunc;
    mspInitArgs = cfg;
    mspDeinitArgs = cfg;
}

/**
 * @brief SPI MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - DMA configuration for transmission request by peripheral
 *           - NVIC configuration for DMA interrupt request enable
 * @param hspi: SPI handle pointer
 * @retval None
 */
extern "C" void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
    if(mspInitFunc != NULL) {
        mspInitFunc(hspi, mspInitArgs);
    }
    else {
        printf("HAL_SPI_MspInit: Please call set_msp_functions to assign SPI MSP functions\n");
    }
}

/**
 * @brief SPI MSP De-Initialization
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO, DMA and NVIC configuration to their default state
 * @param hspi: SPI handle pointer
 * @retval None
 */
extern "C" void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi) {
    if(mspDeinitFunc != NULL) {
        mspDeinitFunc(hspi, mspDeinitArgs);
    }
    else {
        printf("HAL_SPI_MspDeInit: Please call set_msp_functions to assign SPI MSP functions\n");
    }
}

void spi::mspErrorHandler(const char* const function, const char *const message) {
    printf("%s failure: %s\n", function, message);
}
