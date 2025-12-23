# Proyek Tachometer Lanjutan dengan STM32F103CBT6

Proyek ini adalah implementasi tachometer (pengukur kecepatan putaran) tingkat lanjut menggunakan mikrokontroler STM32F103CBT6.

## Deskripsi

Tachometer ini dirancang untuk mengukur kecepatan putaran per menit (RPM) dari sebuah motor, kemungkinan besar motor Brushless DC (BLDC), dengan memanfaatkan sensor Hall. Proyek ini dikembangkan menggunakan STM32CubeIDE dan pustaka HAL dari STMicroelectronics.

Berdasarkan file driver yang ada, proyek ini menggunakan Timer (TIM) dalam mode antarmuka sensor Hall untuk menangkap sinyal dari sensor dan menghitung kecepatan putaran.

## Perangkat Keras

*   **Mikrokontroler:** STM32F103CBT6
*   **Sensor:** Sensor Efek Hall (Hall Effect Sensor) untuk mendeteksi putaran motor.
*   **Lainnya:** Papan pengembangan (seperti Blue Pill atau yang sejenis), motor, dan rangkaian driver motor yang sesuai.

## Perangkat Lunak

*   **IDE:** STM32CubeIDE
*   **Framework:** STM32Cube HAL (Hardware Abstraction Layer)
*   **Driver yang Digunakan:**
    *   RCC (Reset and Clock Control)
    *   TIM (Timer), khususnya untuk antarmuka Sensor Hall
    *   GPIO (General-Purpose Input/Output)
    *   PWR (Power Control)

## Fungsionalitas

1.  **Inisialisasi Sistem:** Mengkonfigurasi clock sistem (SYSCLK, HCLK, PCLK) menggunakan driver RCC.
2.  **Inisialisasi GPIO:** Mengkonfigurasi pin GPIO untuk input dari sensor Hall dan mungkin output lainnya (misalnya, untuk LCD atau UART).
3.  **Konfigurasi Timer:** Timer dikonfigurasi dalam mode antarmuka sensor Hall. Dalam mode ini, timer dapat secara otomatis menangkap sinyal dari tiga sensor Hall dan menghitung periode putaran.
4.  **Perhitungan RPM:** Berdasarkan periode yang diukur oleh timer, perangkat lunak akan menghitung kecepatan dalam RPM.
5.  **Output (Dugaan):** Hasil RPM kemungkinan ditampilkan pada layar LCD atau dikirim melalui komunikasi serial (UART) ke terminal PC. (Fungsionalitas ini tidak dapat dikonfirmasi tanpa `main.c`).

## Cara Menggunakan

1.  Buka proyek ini menggunakan STM32CubeIDE.
2.  Hubungkan perangkat keras sesuai dengan konfigurasi pin di dalam kode (`main.c`).
3.  Bangun (Build) proyek.
4.  Unggah (Upload) file `.elf` atau `.bin` yang dihasilkan ke papan STM32F103CBT6.
5.  Nyalakan sistem dan amati outputnya.
