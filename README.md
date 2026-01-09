# Multiple Producers & Consumers - Blocking Queue

## 📝 Mô tả
Project mô phỏng bài toán **Producer-Consumer** với **multiple threads** sử dụng C và POSIX threads (pthread). Hệ thống bao gồm nhiều producer threads tạo events và nhiều consumer threads xử lý events thông qua một blocking queue được đồng bộ hóa.

## 🎯 Các tính năng chính

### 1. **Multiple Producers & Consumers**
- Hỗ trợ 1-10 producer threads và 1-10 consumer threads
- Work distribution: Producers chia đều công việc tự động
- Thread-safe: Sử dụng mutex và condition variables

### 2. **Blocking Queue**
- **Dung lượng**: 5 slots (QUEUE_CAPACITY)
- **Thread-safe** với mutex protection
- **Blocking operations**:
  - Producer blocked khi queue đầy (5/5)
  - Consumer blocked khi queue rỗng (0/5)
- **Graceful shutdown** với signal broadcast

### 3. **Event Types & Processing Times**
| Loại Event | Ký tự | Mô tả | Thời gian xử lý |
|------------|-------|-------|-----------------|
| **SENSOR** | `s` | Random sensor values (0-99) | 1 giây |
| **TIME** | `t` | Datetime string hiện tại | 3 giây |
| **BUTTON** | `a-z` | Button press counting | 2 giây |
| **SHUTDOWN** | `q` | Terminate signal | 0 giây |

### 4. **Ba Modes hoạt động**
- **Mode 1**: Manual input - Nhập phím từ bàn phím *(chưa implement)*
- **Mode 2**: Auto string - Xử lý chuỗi input tự động *(chưa implement)*
- **Mode 3**: File input - Đọc timed events từ file ✅

## 🎮 Cách sử dụng

### Compile:
```bash
gcc -pthread -o program main.c event.c producer.c consumer.c queue.c -lm
```

### Run:
```bash
./program
```

Chương trình sẽ hỏi:
1. **Số lượng Producers** (1-10)
2. **Số lượng Consumers** (1-10)
3. **Mode** (1/2/3)
4. **Tên file** (nếu chọn Mode 3)

---

## 📋 Chi tiết từng Mode

### Mode 1: Manual Input *(Chưa implement)*
Nhập từng phím từ bàn phím:
- `s` - Tạo SENSOR event
- `t` - Tạo TIME event
- `a-z` - Tạo BUTTON event
- `q` - Shutdown

### Mode 2: Auto String *(Chưa implement)*
Nhập chuỗi ký tự, hệ thống tự động xử lý từng ký tự với delay.

### Mode 3: File Input ✅ **[RECOMMENDED]**

#### Định dạng file:
```
<timestamp> <char>
```

**Ví dụ** (`input_timed.txt`):
```
0 s
0 t
0 a
0 b
0 c
1 d
1 e
2 f
3 g
5 q
```

#### Giải thích:
- **Cột 1**: Timestamp (giây) - Thời điểm tạo event kể từ khi bắt đầu
- **Cột 2**: Ký tự event (`s`/`t`/`a-z`/`q`)

#### Chạy với file:
```bash
echo -e '2\n2\n3\ninput_timed.txt' | ./program
```
Hoặc tương tác:
```bash
./program
# Nhập: 2 (producers)
# Nhập: 2 (consumers)
# Nhập: 3 (mode)
# Nhập: input_timed.txt
```

---

## 🔧 Thread Safety Features

### Mutexes được sử dụng:
1. **`event_count_mutex`**: Bảo vệ global event counters
2. **`button_event_mutex`**: Bảo vệ button_event array và summary
3. **Queue mutex**: Trong BlockingQueue struct

### Condition Variables:
- **`can_produce`**: Signal khi queue có chỗ trống
- **`can_consume`**: Signal khi queue có event

### Critical Sections:
- Event count increment
- Button event count update  
- Queue enqueue/dequeue operations
- Shutdown summary printing (chỉ 1 consumer in ra)

---

## 📊 Output Example

### Chạy với 2 producers × 2 consumers:
```bash
$ echo -e '2\n2\n3\ninput_timed.txt' | ./program

⚙ CONFIGURATION
Enter number of PRODUCER threads (1-10): Enter number of CONSUMER threads (1-10):
✓ Starting: 2 producers × 2 consumers

⚙ MODE SELECTION
  1 - Manual input (self-enter keys)
  2 - Auto mode (enter string directly)
  3 - File input (read from file)
Enter mode (1/2/3): Enter input filename: ✓ File loaded: 13 timed events

[Producer 1] Handling events [0-6]
[Producer 1] Create SENSOR #1 (1s)
[Queue] Enqueue SENSOR #1 -> Queue: 1/5
[Producer 1] Create TIME #1 (3s)
[Queue] Enqueue TIME #1 -> Queue: 2/5
[Producer 1] Create BUTTON #1 'a' (2s)
[Queue] Enqueue BUTTON #1 -> Queue: 3/5
[Producer 1] Create BUTTON #2 'b' (2s)
[Queue] Enqueue BUTTON #2 -> Queue: 4/5
[Producer 1] Create BUTTON #3 'c' (2s)
[Queue] Enqueue BUTTON #3 -> Queue: 5/5
[Consumer 1] Started
[Consumer 1] Dequeue SENSOR #1 <- Queue: 4/5
[Consumer 2] Started
[Consumer 2] Dequeue TIME #1 <- Queue: 3/5
[Producer 2] Create BUTTON #4 'f' (2s)
[Queue] Enqueue BUTTON #4 -> Queue: 4/5
[Queue] FULL - Producer blocked          ← BLOCKING!
[Consumer 1] Process: SENSOR #1 Value: 45
[Consumer 2] Process: TIME #1 2026-01-09 14:40:03
...

========================================
          EVENT SUMMARY
========================================
  Total Events:      15
----------------------------------------
  SENSOR events:    1
  BUTTON events:    10
     'a': 1 times
     'b': 1 times
     'c': 1 times
     'd': 1 times
     ...
  TIME events:      1
  SHUTDOWN events:  3
========================================
```

---

## 🎓 Khái niệm OS đã áp dụng

### 1. **Concurrency & Synchronization**
- Multiple threads chạy đồng thời
- Race condition prevention với mutexes
- Deadlock avoidance

### 2. **Producer-Consumer Pattern**
- Bounded buffer (queue with fixed size = 5)
- Condition variables cho blocking operations
- Work distribution: chia đều events cho producers

### 3. **Thread Management**
- Thread creation với `pthread_create`
- Thread joining với `pthread_join`
- Thread-local data với struct parameters

### 4. **Critical Section Protection**
- Mutex lock/unlock patterns
- Atomic operations cho event counters
- Shared resource protection (queue, counters)

---

## 📁 Cấu trúc File

```
├── main.c          # Entry point, thread creation, file parsing
├── queue.c/h       # Blocking queue implementation
├── producer.c/h    # Producer logic, event creation
├── consumer.c/h    # Consumer logic, event processing
├── event.c/h       # Event structures, counters, mutexes
├── input_timed.txt # Sample timed events file
└── README.md       # Documentation
```

---

## 🐛 Debugging Tips

1. **Queue không đầy?**
   - Tăng số events cùng timestamp trong file
   - Giảm số consumers

2. **Consumers không block?**
   - Giảm số events
   - Tăng processing time (sửa trong producer.c)

3. **Race condition?**
   - Check mutex lock/unlock pairs
   - Verify critical sections

---

## 📝 Notes

- **Mode 1 & 2**: Chưa implement (print placeholder message)
- **Mode 3**: Fully functional với timed events
- **Default processing times**: SENSOR=1s, BUTTON=2s, TIME=3s
- **Queue capacity**: 5 (có thể sửa QUEUE_CAPACITY trong queue.h)
- **Max threads**: 10 producers, 10 consumers (có thể tăng trong producer.h)

---

## 👨‍💻 Author
Operating Systems Course - HDH 2025/1

```
[CONSUMER 2] Processing event...
```
→ Consumer 2 đang xử lý event (mất 3s)

```
[PRODUCER 3] Sending 2 SHUTDOWN events for all consumers
```
→ Producer cuối gửi shutdown cho tất cả consumers

## 📈 Performance Considerations

### Bottlenecks:
1. **Queue size**: 5 slots có thể quá nhỏ
2. **Processing time**: 3s/event có thể tạo backlog
3. **Single queue**: Tất cả consumers dùng chung 1 queue

### Optimizations có thể làm:
- Increase QUEUE_CAPACITY
- Reduce TIME_DELAY_SIMULATION
- Implement priority queue
- Add multiple queues (sharding)
- Thread pool pattern

## 🐛 Known Issues Fixed

### Race Conditions đã sửa:
1. ✅ `event_count` không thread-safe → Thêm mutex
2. ✅ `datetime` buffer dùng chung → Malloc riêng cho mỗi TIME event
3. ✅ `button_event[]` không protected → Thêm mutex
4. ✅ Shutdown logic sai → Gửi 1 event cho mỗi consumer
5. ✅ Summary in nhiều lần → Static flag để in 1 lần

## 📚 Files Structure

```
.
├── main.c          # Entry point, thread management
├── queue.c/h       # Blocking queue implementation  
├── producer.c/h    # Producer logic
├── consumer.c/h    # Consumer logic
├── event.c/h       # Event definitions & utilities
└── README.md       # This file
```

## 🎯 Learning Outcomes

Sau khi hoàn thành project này, bạn đã:
- ✅ Hiểu về Producer-Consumer pattern
- ✅ Thực hành thread synchronization với mutex/cond
- ✅ Xử lý race conditions và critical sections
- ✅ Implement blocking operations
- ✅ Debug concurrent programs
- ✅ Apply OS concepts vào thực tế

---

**Author**: Student  
**Course**: Operating Systems (HDH 2025.1)  
**Version**: 3.0 - Multiple Producers & Consumers
