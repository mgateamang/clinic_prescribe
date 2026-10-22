# AIDANS Doctor e-Prescription Client (Qt6 / QML)

## Executive Summary (For Engineering Leaders & Hiring Managers)

**`clinic_prescribe`** is a specialized clinical application written in **C++20 / Qt6 / QML** designed for attending physicians, specialists, and authorized healthcare providers. It provides a secure, streamlined workflow for writing digital medical prescriptions.

---

## Architectural Highlights

1. **Patient Safety & Clinical Workflow:**
   - Doctors select active practitioners from the clinic roster and specify the target patient ID.
   - Prescriptions are assembled using real-time stock from the clinic formulary, preventing doctors from prescribing medicines that are discontinued or out-of-stock.
   - Specific dosage instructions (e.g. `1 tablet orally twice daily after meals for 10 days`) are captured for each line item.
2. **Cryptographic / Human-Readable Prescription Codes:**
   - On submission to `POST /api/v1/prescriptions`, the server returns a unique prescription reference (`RX-XXXXXX-XXXX`).
   - Doctors can print or provide this code to the patient, which flows directly into the pharmacy Point-of-Sale (`clinic_pos`) module for instant fulfillment.
3. **Responsive MVVM Architecture:**
   - Decoupled `PrescribeController` (C++) coordinates REST networking, payload serialization, and connection states.
   - Declarative QML UI provides responsive form validation, prescription item queuing, and instantaneous user feedback.

---

## Build and Run

```bash
# Configure with Meson
meson setup build

# Compile
meson compile -C build

# Launch application
./build/clinic_prescribe
```
