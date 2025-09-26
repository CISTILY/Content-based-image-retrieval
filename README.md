# 🖼️ Content-Based Image Retrieval (CBIR)

Một hệ thống **truy vấn ảnh dựa trên nội dung (Content-Based Image Retrieval)** được xây dựng bằng **C++ và OpenCV**, cho phép tìm kiếm ảnh trong cơ sở dữ liệu dựa trên đặc trưng thị giác thay vì chỉ dựa vào từ khóa.

## 🚀 Tính năng
- Hỗ trợ nhiều phương pháp trích xuất đặc trưng:
  - 📊 Color Histogram (HSV)
  - 🎨 Color Correlogram
  - 📐 Histogram of Oriented Gradients (HOG)
  - 🔑 SIFT
  - ⚡ ORB
- Hỗ trợ mô hình **Bag of Visual Words (BoVW)** để giảm chiều và tăng hiệu quả tìm kiếm.
- Xây dựng **forward index** để lưu trữ đặc trưng ảnh dưới dạng file `.bin`.
- Giao diện trực quan để:
  - Trích xuất đặc trưng từ tập dữ liệu ảnh
  - Truy vấn ảnh với kết quả sắp xếp theo độ tương đồng

## 🏗️ Kiến trúc hệ thống
1. **Offline Phase (Indexing)**
   - Trích xuất đặc trưng từ ảnh
   - Với SIFT/ORB/HOG → gom cụm bằng **K-means** → BoVW histogram
   - Lưu đặc trưng và vocabulary vào file nhị phân `.bin`

2. **Online Phase (Query)**
   - Người dùng chọn ảnh cần truy vấn
   - Hệ thống trích xuất đặc trưng ảnh truy vấn
   - So sánh với cơ sở dữ liệu bằng các độ đo:
     - Chi-square (Color features)
     - L2 / Euclidean (Local features, BoVW)
   - Sắp xếp và trả về Top-K kết quả

## 🧪 Thí nghiệm
- **Datasets:**
  - CD dataset (99 train, 10 test)
  - TMBuD dataset (100 train, 35 test)
- **Hardware:** i5-9600K, 64GB RAM
- **Metrics:** Extraction Time, Query Time, mAP (Mean Average Precision)

### 🔹 Kết quả chính
- **SIFT + BoVW (500 visual words)** đạt mAP cao nhất (~0.5) nhưng thời gian truy vấn chậm.
- **ORB** nhanh nhất, phù hợp ứng dụng thời gian thực, nhưng độ chính xác thấp hơn.
- **HOG** ổn định nhưng mAP thấp.
- **Color Histogram / Correlogram** trích xuất nhanh, nhưng hiệu quả hạn chế khi ánh sáng thay đổi.

## 📸 Demo giao diện
- Giao diện chọn đặc trưng và trích xuất
- Giao diện truy vấn và hiển thị kết quả Top-K

![Demo GUI](docs/demo_gui.png)

## ⚙️ Môi trường lập trình
- Visual Studio 2022
- C++17
- OpenCV 4.10.0

## 📂 Cấu trúc thư mục
├── Content-based image retrieval/ # Source code
├── release/ # Executable
├── docs/ # Report
└── dataset/ # Generated feature database
    ├── CD_images/ # 109 images of CDs
    └── TBMuD/ # STANDARD parse of original TBMuD with 135 structure images
