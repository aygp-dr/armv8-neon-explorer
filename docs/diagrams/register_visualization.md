# ARM NEON Register Visualization

These diagrams visually represent how ARM NEON registers work and how data is organized within them.

## Basic Register Layout

```mermaid
graph TD
    subgraph "128-bit NEON Vector Register"
        R["V0-V31 (128-bit)"]
    end
    
    subgraph "8-bit Element View (16 elements)"
        B0["Byte 0"] --- B1["Byte 1"] --- B2["..."] --- B15["Byte 15"]
    end
    
    subgraph "16-bit Element View (8 elements)"
        H0["Half 0"] --- H1["Half 1"] --- H2["..."] --- H7["Half 7"]
    end
    
    subgraph "32-bit Element View (4 elements)"
        S0["Single 0"] --- S1["Single 1"] --- S2["Single 2"] --- S3["Single 3"]
    end
    
    subgraph "64-bit Element View (2 elements)"
        D0["Double 0"] --- D1["Double 1"]
    end
    
    R --> B0
    R --> H0
    R --> S0
    R --> D0
    
    classDef default fill:#f9f9f9,stroke:#333,stroke-width:1px;
    classDef register fill:#d1e7dd,stroke:#333,stroke-width:2px;
    class R register;
```

## Data Arrangement Specifiers

```mermaid
graph TD
    subgraph "NEON Register Layout Examples"
        subgraph "8B Arrangement (8 bytes in 64-bit register)"
            B8_0["B0"] --- B8_1["B1"] --- B8_2["B2"] --- B8_3["B3"] --- B8_4["B4"] --- B8_5["B5"] --- B8_6["B6"] --- B8_7["B7"]
        end
        
        subgraph "16B Arrangement (16 bytes in 128-bit register)"
            B16_0["B0"] --- B16_1["B1"] --- B16_2["..."] --- B16_15["B15"]
        end
        
        subgraph "4H Arrangement (4 half-words in 64-bit register)"
            H4_0["H0"] --- H4_1["H1"] --- H4_2["H2"] --- H4_3["H3"]
        end
        
        subgraph "8H Arrangement (8 half-words in 128-bit register)" 
            H8_0["H0"] --- H8_1["H1"] --- H8_2["..."] --- H8_7["H7"]
        end
        
        subgraph "2S Arrangement (2 words in 64-bit register)"
            S2_0["S0"] --- S2_1["S1"]
        end
        
        subgraph "4S Arrangement (4 words in 128-bit register)"
            S4_0["S0"] --- S4_1["S1"] --- S4_2["S2"] --- S4_3["S3"]
        end
        
        subgraph "1D Arrangement (1 double-word in 64-bit register)"
            D1_0["D0"]
        end
        
        subgraph "2D Arrangement (2 double-words in 128-bit register)"
            D2_0["D0"] --- D2_1["D1"]
        end
    end
    
    classDef default fill:#f9f9f9,stroke:#333,stroke-width:1px;
    classDef byte fill:#d1e7dd,stroke:#333,stroke-width:1px;
    classDef half fill:#fff2cc,stroke:#333,stroke-width:1px;
    classDef single fill:#dae8fc,stroke:#333,stroke-width:1px;
    classDef double fill:#ffe6cc,stroke:#333,stroke-width:1px;
    
    class B8_0,B8_1,B8_2,B8_3,B8_4,B8_5,B8_6,B8_7,B16_0,B16_1,B16_2,B16_15 byte;
    class H4_0,H4_1,H4_2,H4_3,H8_0,H8_1,H8_2,H8_7 half;
    class S2_0,S2_1,S4_0,S4_1,S4_2,S4_3 single;
    class D1_0,D2_0,D2_1 double;
```

## SIMD Operation Flow

```mermaid
flowchart LR
    subgraph "Memory"
        A["Array A"]
        B["Array B"]
        C["Result Array"]
    end
    
    subgraph "NEON Registers"
        VA["V0 (from A)"]
        VB["V1 (from B)"]
        VC["V2 (result)"]
    end
    
    A --"vld1q_f32"--> VA
    B --"vld1q_f32"--> VB
    VA --"Input 1"--> OP["SIMD Operation\n(vaddq_f32, vmulq_f32, etc.)"]
    VB --"Input 2"--> OP
    OP --"Output"--> VC
    VC --"vst1q_f32"--> C
    
    style OP fill:#f96,stroke:#333,stroke-width:2px
    style VA fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VB fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VC fill:#d1e7dd,stroke:#333,stroke-width:1px
```

## NEON Register File

```mermaid
graph TD
    subgraph "ARM v8 Register File"
        subgraph "NEON/FP Registers (128-bit)"
            V0["V0/Q0"] --- V1["V1/Q1"] --- V2["..."] --- V31["V31/Q31"]
        end
        
        subgraph "Alternative Views"
            D["D0-D31 (64-bit)"]
            S["S0-S31 (32-bit)"]
            H["H0-H31 (16-bit)"]
            B["B0-B31 (8-bit)"]
        end
        
        V0 --- D
        V0 --- S
        V0 --- H
        V0 --- B
    end
    
    classDef default fill:#f9f9f9,stroke:#333,stroke-width:1px;
    classDef register fill:#d1e7dd,stroke:#333,stroke-width:2px;
    classDef view fill:#fff2cc,stroke:#333,stroke-width:1px;
    
    class V0,V1,V2,V31 register;
    class D,S,H,B view;
```

## Load/Store Operations

```mermaid
graph TD
    subgraph "Memory"
        M1["Memory Address A"]
        M2["Memory Address A+16"]
        M3["Memory Address A+32"]
        M4["Memory Address A+48"]
    end
    
    subgraph "NEON Registers"
        V0["V0"]
        V1["V1"]
        V2["V2"]
        V3["V3"]
    end
    
    subgraph "Load Operations"
        L1["vld1q (1 register)"]
        L4["vld4q (4 registers interleaved)"]
    end
    
    M1 --"Single vector"--> L1 --> V0
    M1 --"Interleaved load"--> L4
    L4 --> V0
    L4 --> V1
    L4 --> V2
    L4 --> V3
    
    classDef memory fill:#dae8fc,stroke:#333,stroke-width:1px;
    classDef register fill:#d1e7dd,stroke:#333,stroke-width:1px;
    classDef operation fill:#fff2cc,stroke:#333,stroke-width:1px;
    
    class M1,M2,M3,M4 memory;
    class V0,V1,V2,V3 register;
    class L1,L4 operation;
```
