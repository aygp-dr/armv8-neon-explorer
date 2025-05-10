# Memory Layout Visualizations

Different memory layouts and their impact on SIMD operations.

## Array of Structures vs. Structure of Arrays

```mermaid
graph TD
    subgraph "Array of Structures (AoS)"
        direction LR
        A1["Struct 1 { X, Y, Z, W }"] --> A2["Struct 2 { X, Y, Z, W }"] --> A3["..."] --> A4["Struct N { X, Y, Z, W }"]
    end
    
    subgraph "Structure of Arrays (SoA)"
        direction LR
        X["X: { X₁, X₂, X₃, ..., Xₙ }"]
        Y["Y: { Y₁, Y₂, Y₃, ..., Yₙ }"]
        Z["Z: { Z₁, Z₂, Z₃, ..., Zₙ }"]
        W["W: { W₁, W₂, W₃, ..., Wₙ }"]
    end
    
    subgraph "SIMD Loading"
        AosLoad["SIMD Load in AoS"]
        SoaLoad["SIMD Load in SoA"]
    end
    
    A1 --> AosLoad
    X --> SoaLoad
    
    classDef struct fill:#dae8fc,stroke:#333,stroke-width:1px;
    classDef array fill:#d1e7dd,stroke:#333,stroke-width:1px;
    classDef load fill:#f96,stroke:#333,stroke-width:2px;
    
    class A1,A2,A3,A4 struct;
    class X,Y,Z,W array;
    class AosLoad,SoaLoad load;
```

## Memory Alignment

```mermaid
graph TD
    subgraph "Aligned Memory Access"
        AStart["Memory Address (16-byte aligned)"] --> A1["16 bytes"] --> A2["16 bytes"] --> A3["16 bytes"]
        
        AStart --> ALoad1["SIMD Load 1"]
        A1 --> ALoad2["SIMD Load 2"]
        A2 --> ALoad3["SIMD Load 3"]
    end
    
    subgraph "Unaligned Memory Access"
        UStart["Memory Address (unaligned)"] --> U1["First 16 bytes (partial)"] --> U2["Next 16 bytes (crosses boundary)"] --> U3["..."]
        
        UStart --> ULoad1["SIMD Load (may need additional processing)"]
    end
    
    classDef aligned fill:#d1e7dd,stroke:#333,stroke-width:1px;
    classDef unaligned fill:#f8cecc,stroke:#333,stroke-width:1px;
    classDef load fill:#f96,stroke:#333,stroke-width:2px;
    
    class AStart,A1,A2,A3 aligned;
    class UStart,U1,U2,U3 unaligned;
    class ALoad1,ALoad2,ALoad3,ULoad1 load;
```

## Interleaved vs. Planar Image Data

```mermaid
graph TD
    subgraph "Interleaved RGB Image Data"
        direction LR
        I1["R₁G₁B₁"] --> I2["R₂G₂B₂"] --> I3["R₃G₃B₃"] --> I4["..."]
    end
    
    subgraph "Planar RGB Image Data"
        direction LR
        P1["R₁R₂R₃...Rₙ"]
        P2["G₁G₂G₃...Gₙ"]
        P3["B₁B₂B₃...Bₙ"]
    end
    
    subgraph "SIMD Operations"
        Ideload["De-interleave (VLD3)"]
        Iload["Process interleaved"]
        Pload["Process planar"]
    end
    
    I1 --> Ideload
    Ideload --> Pload
    I1 --> Iload
    P1 --> Pload
    
    classDef interleaved fill:#f8cecc,stroke:#333,stroke-width:1px;
    classDef planar fill:#d1e7dd,stroke:#333,stroke-width:1px;
    classDef op fill:#f96,stroke:#333,stroke-width:2px;
    
    class I1,I2,I3,I4 interleaved;
    class P1,P2,P3 planar;
    class Ideload,Iload,Pload op;
```

## Cache Considerations

```mermaid
graph TD
    subgraph "Memory Hierarchy"
        direction TB
        L1["L1 Cache (smallest, fastest)"]
        L2["L2 Cache (medium)"]
        L3["L3 Cache (largest)"]
        RAM["Main Memory (DRAM)"]
    end
    
    subgraph "SIMD Access Patterns"
        Sequential["Sequential Access (cache-friendly)"]
        Random["Random Access (cache-unfriendly)"]
        Strided["Strided Access (varies)"]
    end
    
    RAM --> L3 --> L2 --> L1 --> Sequential
    RAM -.-> Random
    
    classDef cache fill:#dae8fc,stroke:#333,stroke-width:1px;
    classDef memory fill:#d5e8d4,stroke:#333,stroke-width:1px;
    classDef pattern fill:#f96,stroke:#333,stroke-width:2px;
    
    class L1,L2,L3 cache;
    class RAM memory;
    class Sequential,Random,Strided pattern;
```
