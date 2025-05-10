# SIMD Operation Visualizations

Visual representations of how SIMD operations work compared to scalar operations.

## Vector Addition

```mermaid
graph LR
    subgraph "Scalar Addition (4 operations)"
        A1["A[0]"] --> Add1("+") --> C1["C[0]"]
        B1["B[0]"] --> Add1
        
        A2["A[1]"] --> Add2("+") --> C2["C[1]"]
        B2["B[1]"] --> Add2
        
        A3["A[2]"] --> Add3("+") --> C3["C[2]"]
        B3["B[2]"] --> Add3
        
        A4["A[3]"] --> Add4("+") --> C4["C[3]"]
        B4["B[3]"] --> Add4
    end
    
    subgraph "SIMD Addition (1 operation)"
        VA["A[0:3]"] --> VAdd("+") --> VC["C[0:3]"]
        VB["B[0:3]"] --> VAdd
    end
    
    style VAdd fill:#f96,stroke:#333,stroke-width:2px
    style VA fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VB fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VC fill:#d1e7dd,stroke:#333,stroke-width:1px
```

## Vector Dot Product

```mermaid
graph TD
    subgraph "Scalar Dot Product"
        direction LR
        A1["A[0]"] --> Mul1("×") --> P1["Product 1"]
        B1["B[0]"] --> Mul1
        
        A2["A[1]"] --> Mul2("×") --> P2["Product 2"]
        B2["B[1]"] --> Mul2
        
        A3["A[2]"] --> Mul3("×") --> P3["Product 3"]
        B3["B[2]"] --> Mul3
        
        A4["A[3]"] --> Mul4("×") --> P4["Product 4"]
        B4["B[3]"] --> Mul4
        
        P1 --> Sum("Sum")
        P2 --> Sum
        P3 --> Sum
        P4 --> Sum
        
        Sum --> Result["Result"]
    end
    
    subgraph "SIMD Dot Product"
        direction LR
        VA["A[0:3]"] --> VMul("×") --> VP["Products"]
        VB["B[0:3]"] --> VMul
        
        VP --> HSumV("Horizontal Sum") --> VResult["Result"]
    end
    
    style VMul fill:#f96,stroke:#333,stroke-width:2px
    style HSumV fill:#f96,stroke:#333,stroke-width:2px
    style VA fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VB fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VP fill:#d1e7dd,stroke:#333,stroke-width:1px
```

## Vector Interleaving Operation

```mermaid
graph LR
    subgraph "Before ZIP"
        A["A: [A0, A1, A2, A3]"]
        B["B: [B0, B1, B2, B3]"]
    end
    
    subgraph "After ZIP1"
        Res1["[A0, B0, A1, B1]"]
    end
    
    subgraph "After ZIP2"
        Res2["[A2, B2, A3, B3]"]
    end
    
    A --> Op1["ZIP"] --> Res1
    B --> Op1
    
    A --> Op2["ZIP"] --> Res2
    B --> Op2
    
    style Op1 fill:#f96,stroke:#333,stroke-width:2px
    style Op2 fill:#f96,stroke:#333,stroke-width:2px
    style A fill:#d1e7dd,stroke:#333,stroke-width:1px
    style B fill:#d1e7dd,stroke:#333,stroke-width:1px
```

## Vector Element Extraction

```mermaid
graph LR
    subgraph "Source Vectors"
        A["A: [A0, A1, A2, A3]"]
        B["B: [B0, B1, B2, B3]"]
    end
    
    subgraph "After EXT (index=2)"
        Res["[A2, A3, B0, B1]"]
    end
    
    A --> Op1["EXT (2)"] --> Res
    B --> Op1
    
    style Op1 fill:#f96,stroke:#333,stroke-width:2px
    style A fill:#d1e7dd,stroke:#333,stroke-width:1px
    style B fill:#d1e7dd,stroke:#333,stroke-width:1px
```

## Fused Multiply-Add

```mermaid
graph TD
    subgraph "Scalar Fused Multiply-Add"
        direction LR
        A1["A[0]"] --> Mul1("×") --> P1
        B1["B[0]"] --> Mul1
        P1["Product 1"] --> Add1("+") --> R1["Result 1"]
        C1["C[0]"] --> Add1
        
        A2["A[1]"] --> Mul2("×") --> P2
        B2["B[1]"] --> Mul2
        P2["Product 2"] --> Add2("+") --> R2["Result 2"]
        C2["C[1]"] --> Add2
        
        A3["A[2]"] --> Mul3("×") --> P3
        B3["B[2]"] --> Mul3
        P3["Product 3"] --> Add3("+") --> R3["Result 3"]
        C3["C[2]"] --> Add3
        
        A4["A[3]"] --> Mul4("×") --> P4
        B4["B[3]"] --> Mul4
        P4["Product 4"] --> Add4("+") --> R4["Result 4"]
        C4["C[3]"] --> Add4
    end
    
    subgraph "SIMD Fused Multiply-Add"
        direction LR
        VA["A[0:3]"] --> FMA("A×B+C") --> VR["Result[0:3]"]
        VB["B[0:3]"] --> FMA
        VC["C[0:3]"] --> FMA
    end
    
    style FMA fill:#f96,stroke:#333,stroke-width:2px
    style VA fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VB fill:#d1e7dd,stroke:#333,stroke-width:1px
    style VC fill:#d1e7dd,stroke:#333,stroke-width:1px
```
