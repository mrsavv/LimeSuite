/**
@file	LMS7002M.h
@author Lime Microsystems (www.limemicro.com)
@brief	LMS7002M transceiver configuration interface
*/

#ifndef LMS7API_H
#define LMS7API_H

#include "LMS7002M_parameters.h"
#include <cstdint>

#include <sstream>

namespace lime{
class IConnection;
class LMS7002M_RegistersMap;
class CalibrationCache;
class MCU_BD;

typedef double float_type;

class LMS7002M
{
public:
    enum
    {
        Rx, Tx
    };

	LMS7002M();

    /*!
     * Set the connection for the LMS7002M driver.
     * \param port the connection interface
     * \param devIndex which RFIC index (default 0 for most devices)
     */
    void SetConnection(IConnection* port, const size_t devIndex = 0);

    IConnection *GetConnection(void) const
    {
        return controlPort;
    }

	virtual ~LMS7002M();

    /*!
     * Enum for configuring the channel selection.
     * @see MAC register
     */
    enum Channel
    {
        ChA = 1,
        ChB = 2,
        ChAB = 3,
        ChSXR = 1, //SXR register space
        ChSXT = 2, //SXT register space
    };

    /*!
     * Set the selected channel (MAC).
     * The API calls will reflect this channel.
     */
    void SetActiveChannel(const Channel ch);

    /*!
     * Get the selected channel (MAC).
     * The API calls will reflect this channel.
     */
    Channel GetActiveChannel(bool fromChip = true);

    /*!
     * Get the channel selected by the RFIC index (devIndex),
     * and by the currently selected RF channel A/B (MAC).
     * Example when devIndex == 0, return 0 for chA, 1 for chB.
     */
    size_t GetActiveChannelIndex(bool fromChip = true);

    /*!
     * Enable/disable the selected channel.
     * This powers on or off all of the respective hardware
     * for a given channel A or B: TSP, BB, and RF sections.
     * @param isTx true for the transmit size, false for receive
     * @param enable true to enable, false to disable
     */
    int EnableChannel(const bool isTx, const bool enable);

    ///@name Registers writing and reading
    int UploadAll();
    int DownloadAll();
    bool IsSynced();

	int ResetChip();

    /*!
     * Perform soft-reset sequence over SPI
     */
    int SoftReset();

	int LoadConfig(const char* filename);
	int SaveConfig(const char* filename);
    ///@}

    ///@name Registers writing and reading
    uint16_t Get_SPI_Reg_bits(const LMS7Parameter &param, bool fromChip = false);
    uint16_t Get_SPI_Reg_bits(uint16_t address, uint8_t msb, uint8_t lsb, bool fromChip = false);
    int Modify_SPI_Reg_bits(const LMS7Parameter &param, const uint16_t value, bool fromChip = false);
    int Modify_SPI_Reg_bits(uint16_t address, uint8_t msb, uint8_t lsb, uint16_t value, bool fromChip = false);
    int SPI_write(uint16_t address, uint16_t data);
    uint16_t SPI_read(uint16_t address, bool fromChip = false, int *status = 0);
    int RegistersTest();
    ///@}

    ///@name Calibration protection:
    ///Called internally by calibration and cgen API.
    ///Call externally when performing multiple cals.
    ///Safe to next calls to enter and exit.
    ///Always match calls to enter+exit.
    void EnterSelfCalibration(void);
    void ExitSelfCalibration(void);
    ///@}

    ///@name Transmitter, Receiver calibrations
    /*!
     * Store the digital corrections for the current channel.
     * The corrections are written to the calibration database.
     * This call is typically called by the self-cal utilities.
     * @param isTx true for transmit, false for receive
     * @return 0 for success otherwise an error code
     */
    int StoreDigitalCorrections(const bool isTx);

    /*!
     * Apply digital corrections for the current channel.
     * Lookup corrections in the calibration database
     * using the current synthesizer frequency. The call
     * will fail if a calibration point cannot be found.
     * @param isTx true for transmit, false for receive
     * @return 0 for success otherwise an error code
     */
    int ApplyDigitalCorrections(const bool isTx);

    int CalibrateRx(float_type bandwidth, const bool useExtLoopback = false);
    int CalibrateTx(float_type bandwidth, const bool useExtLoopback = false);
    ///@}

    ///@name Filters tuning
	int TuneTxFilterFixed(const float_type fixedBandwidth);
	int TuneTxFilter(const float_type bandwidth);
	int TuneRxFilter(const float_type rx_lpf_freq_RF);
    ///@}

    ///@name High level gain configuration

    /*!
     * Set the RX PGA gain in dB
     * @param gain in dB range -12.0, 19.0 dB
     * @return 0 for success, else error
     */
    int SetRBBPGA_dB(const float_type gain);

    //! Get the actual RX PGA gain in dB
    float_type GetRBBPGA_dB(void);

    /*!
     * Set the RX LNA gain in dB
     * @param gain in dB range 0.0, 30.0 dB
     * @return 0 for success, else error
     */
    int SetRFELNA_dB(const float_type gain);

    //! Get the actual RX LNA gain in dB
    float_type GetRFELNA_dB(void);

    /*!
     * Set the RX loopback LNA gain in dB
     * @param gain in dB range 0.0, 40.0 dB
     * @return 0 for success, else error
     */
    int SetRFELoopbackLNA_dB(const float_type gain);

    //! Get the actual RX loopback LNA gain in dB
    float_type GetRFELoopbackLNA_dB(void);

    /*!
     * Set the RX TIA gain in dB
     * @param gain in dB range 0.0, 12.0 dB
     * @return 0 for success, else error
     */
    int SetRFETIA_dB(const float_type gain);

    //! Get the actual RX TIA gain in dB
    float_type GetRFETIA_dB(void);

    /*!
     * Set the TX PAD gain in dB
     * @param gain in dB range -52.0, 0.0 dB
     * @return 0 for success, else error
     */
    int SetTRFPAD_dB(const float_type gain);

    //! Get the actual TX PAD gain in dB
    float_type GetTRFPAD_dB(void);

    /*!
     * Set the TX loopback PAD gain in dB
     * @param gain in dB range -4.3, 0.0 dB
     * @return 0 for success, else error
     */
    int SetTRFLoopbackPAD_dB(const float_type gain);

    //! Get the actual TX loopback PAD gain in dB
    float_type GetTRFLoopbackPAD_dB(void);

    ///@}

    ///@name RF selection
    enum PathRFE
    {
        PATH_RFE_NONE = 0,
        PATH_RFE_LNAH = int('H'),
        PATH_RFE_LNAL = int('L'),
        PATH_RFE_LNAW = int('W'),
        PATH_RFE_LB1 = 1,
        PATH_RFE_LB2 = 2,
    };

    //! Set the RFE input path.
    int SetPathRFE(PathRFE path);

    //! Get the currently set RFE path
    PathRFE GetPathRFE(void);

    /*!
     * Set the TRF Band selection.
     * @param band 1 or 2
     */
    int SetBandTRF(const int band);

    /*!
     * Get the TRF Band selection.
     * @return the band 1 or 2
     */
    int GetBandTRF(void);

    /*!
     * Update the external band selection by calling
     * UpdateExternalBandSelect() on the connection object.
     * This is called automatically by the LMS7002M driver,
     * but can also be called manually by the user.
     */
    void UpdateExternalBandSelect(void);

    ///@}

    ///@name CGEN and PLL
	void SetReferenceClk_SX(bool tx, float_type freq_Hz);
	float_type GetReferenceClk_SX(bool tx);
	float_type GetFrequencyCGEN();
	int SetFrequencyCGEN(float_type freq_Hz, const bool retainNCOfrequencies = false);
	bool GetCGENLocked(void);
	float_type GetFrequencySX(bool tx);
	int SetFrequencySX(bool tx, float_type freq_Hz);
	bool GetSXLocked(bool tx);
    ///VCO modules available for tuning
    enum VCO_Module
    {
        VCO_CGEN, VCO_SXR, VCO_SXT
    };
    int TuneVCO(VCO_Module module);
    ///@}

    ///@name TSP
	int LoadDC_REG_IQ(bool tx, int16_t I, int16_t Q);
	int SetNCOFrequency(bool tx, uint8_t index, float_type freq_Hz);
	float_type GetNCOFrequency(bool tx, uint8_t index, bool fromChip = true);
    int SetNCOPhaseOffsetForMode0(bool tx, float_type angle_Deg);
	int SetNCOPhaseOffset(bool tx, uint8_t index, float_type angle_Deg);
	float_type GetNCOPhaseOffset_Deg(bool tx, uint8_t index);
	int SetGFIRCoefficients(bool tx, uint8_t GFIR_index, const int16_t *coef, uint8_t coefCount);
	int GetGFIRCoefficients(bool tx, uint8_t GFIR_index, int16_t *coef, uint8_t coefCount);
    float_type GetReferenceClk_TSP(bool tx);
    ///@}

    int SetInterfaceFrequency(float_type cgen_freq_Hz, const uint8_t interpolation, const uint8_t decimation);

    //! Get the sample rate in Hz
    float_type GetSampleRate(bool tx);

    ///@name LML
    enum LMLSampleSource
    {
        AI = 0,
        AQ = 1,
        BI = 2,
        BQ = 3,
    };

    /*!
     * Set the LML sample positions in the RF to baseband direction.
     */
    void ConfigureLML_RF2BB(
        const LMLSampleSource s0,
        const LMLSampleSource s1,
        const LMLSampleSource s2,
        const LMLSampleSource s3);

    /*!
     * Set the LML sample positions in the baseband to RF direction.
     */
    void ConfigureLML_BB2RF(
        const LMLSampleSource s0,
        const LMLSampleSource s1,
        const LMLSampleSource s2,
        const LMLSampleSource s3);
    ///@}

    /*!
     * Set enable for the RX DC removal filter.
     * @param enable true to enable DC removal
     * @return 0 for success for error condition
     */
    int SetRxDCRemoval(const bool enable);

    /*!
     * Get the RX DC removal filter enabled.
     */
    bool GetRxDCRemoval(void);

    /*!
     * Set the TX DC offset adjustment.
     * @param I the real adjustment [+1.0, -1.0]
     * @param Q the imaginary adjustment [+1.0, -1.0]
     * @return 0 for success for error condition
     */
    int SetTxDCOffset(const float_type I, const float_type Q);

    /*!
     * Readback the TX DC offset adjustment.
     * @param [out] I the real adjustment [+1.0, -1.0]
     * @param [out] Q the imaginary adjustment [+1.0, -1.0]
     */
    void GetTxDCOffset(float_type &I, float_type &Q);

    /*!
     * Set the IQ imbalance correction.
     * @param tx true for tx, false for rx
     * @param phase the phase adjustment [+pi, -pi]
     * @param gainI the real gain adjustment [+1.0, 0.0]
     * @param gainQ the imaginary gain adjustment [+1.0, 0.0]
     */
    int SetIQBalance(const bool tx, const float_type phase, const float_type gainI, const float_type gainQ);

    /*!
     * Get the IQ imbalance correction.
     * @param tx true for tx, false for rx
     * @param [out] phase the phase adjustment [+pi, -pi]
     * @param [out] gainI the real gain adjustment [+1.0, 0.0]
     * @param [out] gainQ the imaginary gain adjustment [+1.0, 0.0]
     */
    void GetIQBalance(const bool tx, float_type &phase, float_type &gainI, float_type &gainQ);

    ///enumeration to indicate module registers intervals
    enum MemorySection
    {
        LimeLight = 0, EN_DIR, AFE, BIAS, XBUF, CGEN, LDO, BIST, CDS,
        TRF, TBB, RFE, RBB, SX, TxTSP,
        TxNCO, TxGFIR1, TxGFIR2, TxGFIR3a, TxGFIR3b, TxGFIR3c,
        RxTSP, RxNCO, RxGFIR1, RxGFIR2, RxGFIR3a, RxGFIR3b, RxGFIR3c,
        MEMORY_SECTIONS_COUNT
    };
    virtual int SetDefaults(MemorySection module);

    static float_type gVCO_frequency_table[3][2];
    static float_type gCGEN_VCO_frequencies[2];

    void EnableValuesCache(bool enabled = true);
    bool IsValuesCacheEnabled();
    MCU_BD* GetMCUControls() const;
    void EnableCalibrationByMCU(bool enabled);
    float_type GetTemperature();
protected:
    bool mCalibrationByMCU;
    MCU_BD *mcuControl;
    bool useCache;
    CalibrationCache *mValueCache;
    LMS7002M_RegistersMap *mRegistersMap;
    static const uint16_t readOnlyRegisters[];
    static const uint16_t readOnlyRegistersMasks[];

    LMS7002M_RegistersMap *BackupRegisterMap(void);
    void RestoreRegisterMap(LMS7002M_RegistersMap *backup);

    uint16_t MemorySectionAddresses[MEMORY_SECTIONS_COUNT][2];
    ///@name Algorithms functions
    void BackupAllRegisters();
    void RestoreAllRegisters();
    uint32_t GetRSSI();
    uint32_t GetAvgRSSI(const int avgCount);
    void SetRxDCOFF(int8_t offsetI, int8_t offsetQ);
    void CalibrateRxDC_RSSI();
    void CalibrateTxDC_RSSI(const float_type bandwidth);

    int CalibrateTxSetup(const float_type bandwidth_Hz, const bool useExtLoopback);
    int CalibrateRxSetup(const float_type bandwidth_Hz, const bool useExtLoopback);
    int CheckSaturationRx(const float_type bandwidth_Hz, const bool useExtLoopback);
    int CheckSaturationTxRx(const float_type bandwidth_Hz, const bool useExtLoopback);

    void CoarseSearch(const uint16_t addr, const uint8_t msb, const uint8_t lsb, int16_t &value, const uint8_t maxIterations);
    void FineSearch(const uint16_t addrI, const uint8_t msbI, const uint8_t lsbI, int16_t &valueI, const uint16_t addrQ, const uint8_t msbQ, const uint8_t lsbQ, int16_t &valueQ, const uint8_t fieldSize);
    int RxFilterSearch(const LMS7Parameter &param, const uint32_t rssi_3dB, uint8_t rssiAvgCnt, const int stepLimit);
    int TxFilterSearch(const LMS7Parameter &param, const uint32_t rssi_3dB, uint8_t rssiAvgCnt, const int stepLimit);
    int TxFilterSearch_LAD(const LMS7Parameter &param, uint32_t *rssi_3dB, uint8_t rssiAvgCnt, const int stepLimit, const int NCO_index);
    int TxFilterSearch_S5(const LMS7Parameter &param, const uint32_t rssi_3dB, uint8_t rssiAvgCnt, const int stepLimit);

    int TuneRxFilterSetup(const float_type rx_lpf_freq);
    int TuneTxFilterFixedSetup();
    int TuneTxFilterSetup(const float_type tx_lpf_freq);

    int RegistersTestInterval(uint16_t startAddr, uint16_t endAddr, uint16_t pattern, std::stringstream &ss);
    int SPI_write_batch(const uint16_t* spiAddr, const uint16_t* spiData, uint16_t cnt);
    int SPI_read_batch(const uint16_t* spiAddr, uint16_t* spiData, uint16_t cnt);
    int Modify_SPI_Reg_mask(const uint16_t *addr, const uint16_t *masks, const uint16_t *values, uint8_t start, uint8_t stop);
    ///@}

    enum LogType
    {
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_DATA
    };
    virtual void Log(const char* text, LogType type);

    ///port used for communicating with LMS7002M
    IConnection* controlPort;
    int addrLMS7002M;
    size_t mdevIndex;
    size_t mSelfCalDepth;

    int LoadConfigLegacyFile(const char* filename);
};


/*!
 * Helper class to enter a calibration upon construction,
 * and to automatically exit calibration upon exit.
 */
class LMS7002M_SelfCalState
{
public:
    LMS7002M_SelfCalState(LMS7002M *rfic);
    ~LMS7002M_SelfCalState(void);

private:
    LMS7002M *rfic;
};


}
#endif
