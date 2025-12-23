# Proyek Tachometer Lanjutan dengan STM32F103CBT6

Proyek ini adalah implementasi tachometer digital menggunakan STM32F103CBT6 yang menampilkan data pada modul LED & Key TM1638 serta mengirimkan log data melalui USB (Virtual COM Port).

## Deskripsi

Tachometer ini mengukur kecepatan putaran (RPM) dan frekuensi sinyal input menggunakan fitur *Input Capture* pada Timer. Data ditampilkan secara *real-time* pada 7-segment display modul TM1638. Proyek ini juga mendukung fitur logging data ke PC melalui USB CDC.

## Perangkat Keras

*   **Mikrokontroler:** STM32F103CBT6 (Blue Pill).
*   **Display & Input:** Modul TM1638 (8 digit 7-segment, 8 LED, 8 Tombol).
*   **Input Sinyal:** Sinyal pulsa digital (misal: dari sensor Hall, Optocoupler, atau FG signal generator) yang terhubung ke **TIM2 Channel 1**.
*   **Komunikasi:** USB (konektor USB onboard STM32).

## Perangkat Lunak

*   **IDE:** STM32CubeIDE
*   **Framework:** STM32Cube HAL
*   **Fitur Utama:**
    *   **TIM2 Input Capture:** Menghitung periode antar pulsa untuk presisi tinggi.
    *   **DMA SPI:** Mengirim data ke display TM1638 tanpa membebani CPU.
    *   **USB CDC:** Mengirim data serial ke PC.

## Fungsionalitas & Kontrol

Program memiliki fitur *auto-zero* (RPM kembali ke 0 jika tidak ada sinyal selama 3 detik) dan filter *software* untuk mengabaikan *noise* frekuensi tinggi (>12.000 RPM).

### Fungsi Tombol pada TM1638:

| Tombol | Fungsi | Deskripsi |
| :--- | :--- | :--- |
| **S1** | Mode RPM | Menampilkan nilai RPM saat ini. |
| **S2** | Mode Frekuensi | Menampilkan frekuensi input dalam Hz. |
| **S3** | Mode Counter | Menampilkan total jumlah pulsa yang terhitung. |
| **S4** | Max RPM | Menampilkan nilai RPM tertinggi yang pernah tercatat. |
| **S5** | Min RPM | Menampilkan nilai RPM terendah (selain 0). |
| **S6** | Reset | Mereset nilai Counter, Max RPM, dan Min RPM ke 0. |

## Output Data

1.  **Display TM1638:** Menampilkan angka sesuai mode yang dipilih.
2.  **USB Serial Monitor:** Mengirim string dengan format:
    `RPM: [nilai] | Freq: [nilai] Hz | Total: [nilai]`

## Cara Menggunakan

1.  Buka proyek ini menggunakan STM32CubeIDE.
2.  Hubungkan modul TM1638 ke pin SPI yang sesuai dan sinyal input ke pin Timer Input Capture.
3.  Bangun (Build) proyek.
4.  Unggah (Upload) file `.elf` atau `.bin` yang dihasilkan ke papan STM32F103CBT6.
5.  Nyalakan sistem, tekan tombol pada TM1638 untuk mengganti mode tampilan.
