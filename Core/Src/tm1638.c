#include "tm1638.h"
#include "main.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

// Buffer untuk menampung paket data DMA
// [0] = Command Address Set (0xC0)
// [1..16] = Data display (Seg1..Seg16)
static uint8_t dma_buffer[17];

// Status apakah sedang transfer (untuk mencegah overlapping call)
static volatile uint8_t is_transferring = 0;

// Definisi font (sama seperti sebelumnya)
const uint8_t TM1638_FONT[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x00, 0x40};

// Helper Macros
#define STB_LOW() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define STB_HIGH() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

// --- Fungsi Init (Blocking tidak masalah karena cuma sekali) ---
void TM1638_Init(void)
{
    STB_HIGH();
    // Kirim command display control (Blocking)
    uint8_t cmd = 0x8F; // Display ON, Brightness Max
    STB_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    STB_HIGH();

    // Set Data Command ke Auto Increment Address (Blocking)
    // Ini penting agar DMA bisa kirim stream data berturut-turut
    cmd = 0x40;
    STB_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    STB_HIGH();

    TM1638_Clear();
}

// --- Fungsi Kirim DMA ---
void TM1638_SendDMA(const char *str, uint8_t leds)
{
    if (is_transferring)
        return; // Jangan kirim jika sebelumnya belum selesai

    // 1. Siapkan Buffer
    dma_buffer[0] = 0xC0; // Command: Set Address to 00H

    // 2. Isi Buffer Data Display (Mapping String ke 7-Segment)
    uint8_t len = strlen(str);
    for (int i = 0; i < 8; i++)
    {
        uint8_t seg_data = 0;
        if (i < len)
        {
            char c = str[i];
            if (c >= '0' && c <= '9')
                seg_data = TM1638_FONT[c - '0'];
            else if (c >= 'A' && c <= 'F')
                seg_data = TM1638_FONT[c - 'A' + 10];
            else if (c == '-')
                seg_data = 0x40;
            else if (c == ' ')
                seg_data = 0x00;
        }

        // Mapping TM1638: Address genap untuk 7-segment, ganjil untuk LED
        // Buffer index 1 correspond to Addr 00H (7-Seg 1)
        // Buffer index 2 correspond to Addr 01H (LED 1)
        // dst...

        dma_buffer[1 + (i * 2)] = seg_data;            // 7-Segment
        dma_buffer[1 + (i * 2) + 1] = (leds >> i) & 1; // LED
    }

    // 3. Mulai Transaksi DMA
    is_transferring = 1;
    STB_LOW(); // Tarik STB Low sebelum DMA mulai

    // Kirim 17 byte (1 Cmd + 16 Data)
    if (HAL_SPI_Transmit_DMA(&hspi1, dma_buffer, 17) != HAL_OK)
    {
        is_transferring = 0;
        STB_HIGH(); // Error recovery
    }
}

// --- Callback saat DMA Selesai ---
// Fungsi ini dipanggil otomatis oleh HAL Interrupt Handler
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        STB_HIGH(); // Kembalikan STB ke High setelah semua data terkirim
        is_transferring = 0;
    }
}

// --- Fungsi Baca Tombol (Tetap Blocking) ---
// Baca tombol jarang dilakukan dan butuh respon cepat untuk logic,
// jadi blocking 4 byte (sangat cepat) biasanya lebih aman daripada DMA RX yang kompleks.
uint8_t TM1638_ReadButtons(void)
{
    // ... Gunakan kode blocking read buttons yang sebelumnya ...
    // Karena membaca hanya 5 byte total, overhead DMA setup mungkin lebih lambat
    // daripada blocking SPI di 2-4MHz.
    uint8_t buttons[4] = {0};
    uint8_t cmd = 0x42;
    uint8_t final_key = 0;

    STB_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);

    // Ganti arah ke Input (Trik Half Duplex)
    __HAL_SPI_DISABLE(&hspi1);
    SPI1->CR1 &= ~SPI_CR1_BIDIOE;
    __HAL_SPI_ENABLE(&hspi1);

    HAL_SPI_Receive(&hspi1, buttons, 4, 10);

    __HAL_SPI_DISABLE(&hspi1);
    SPI1->CR1 |= SPI_CR1_BIDIOE;
    __HAL_SPI_ENABLE(&hspi1);
    STB_HIGH();

    // Decoding logic (sama seperti sebelumnya)
    final_key |= (buttons[0] & 0x01) ? 0x01 : 0;
    final_key |= (buttons[1] & 0x01) ? 0x02 : 0;
    final_key |= (buttons[2] & 0x01) ? 0x04 : 0;
    final_key |= (buttons[3] & 0x01) ? 0x08 : 0;
    final_key |= (buttons[0] & 0x10) ? 0x10 : 0;
    final_key |= (buttons[1] & 0x10) ? 0x20 : 0;
    final_key |= (buttons[2] & 0x10) ? 0x40 : 0;
    final_key |= (buttons[3] & 0x10) ? 0x80 : 0;

    return final_key;
}

// Dummy functions agar tidak error saat compile jika masih dipanggil
void TM1638_Clear(void)
{
    TM1638_SendDMA("        ", 0);
}

void TM1638_DisplayString(const char *str)
{
    // Kita asumsikan LEDs mati kalau panggil ini, atau simpan state global LEDs
    TM1638_SendDMA(str, 0);
}

void TM1638_SetLEDs(uint8_t data)
{
    // Perlu logic tambahan jika ingin update LED tanpa hapus teks
    // Tapi untuk simplifikasi DMA, kirim ulang semua paket lebih mudah.
}