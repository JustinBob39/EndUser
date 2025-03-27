#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <cassert>
#include <deque>

#include "CRC24Q.h"

class Deserializer {
    struct Time {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;

        enum TimeIndex {
            Year, Month, Day, Hour, Minute, Second
        };

        static constexpr int TIME_INDEX_BC[] = {
            7, 4, 5, 5, 6, 6
        };

        static void set_int(const std::deque<bool> &frame_bool, const int start, const int bc, int &val) {
            int offset = start;
            int mask = 1 << (bc - 1);
            for (int i = 0; i < bc; ++i) {
                if (frame_bool[offset]) {
                    val |= mask;
                }
                ++offset;
                mask >>= 1;
            }
        }

        void set_year(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Year], year);
        }

        void set_month(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Month], month);
        }

        void set_day(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Day], day);
        }

        void set_hour(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Hour], hour);
        }

        void set_minute(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Minute], minute);
        }

        void set_second(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, TIME_INDEX_BC[Second], second);
        }

        void set_time(const std::deque<bool> &frame_bool, const int start) {
            int offset = start;
            set_year(frame_bool, offset);
            offset += TIME_INDEX_BC[Year];
            set_month(frame_bool, offset);
            offset += TIME_INDEX_BC[Month];
            set_day(frame_bool, offset);
            offset += TIME_INDEX_BC[Day];
            set_hour(frame_bool, offset);
            offset += TIME_INDEX_BC[Hour];
            set_minute(frame_bool, offset);
            offset += TIME_INDEX_BC[Minute];
            set_second(frame_bool, offset);
            // offset += TIME_INDEX_BC[Second];
        }

        [[nodiscard]] std::string to_string() const {
            return "year: " + std::to_string(year) + '\n'
                   + "month: " + std::to_string(month) + '\n'
                   + "day: " + std::to_string(day) + '\n'
                   + "hour: " + std::to_string(hour) + '\n'
                   + "minute: " + std::to_string(minute) + '\n'
                   + "second: " + std::to_string(second) + '\n';
        }
    };

    struct SubEntry {
        int childId;
        Time childEventTime;
        int childValueFirst;
        int childValueSecond;
        int childDuration;

        enum SubEntryIndex {
            ChildId, ChildEventTime, ChildValueFirst, ChildValueSecond, ChildDuration
        };

        static constexpr int SUB_ENTRY_INDEX_BC[] = {
            5, 33, 18, 16, 2
        };

        static void set_int(const std::deque<bool> &frame_bool, const int start, const int bc, int &val) {
            int offset = start;
            int mask = 1 << (bc - 1);
            for (int i = 0; i < bc; ++i) {
                if (frame_bool[offset]) {
                    val |= mask;
                }
                ++offset;
                mask >>= 1;
            }
        }

        void set_childId(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, SUB_ENTRY_INDEX_BC[ChildId], childId);
        }

        void set_childEventTime(const std::deque<bool> &frame_bool, const int start) {
            childEventTime.set_time(frame_bool, start);
        }

        void set_childValueFirst(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, SUB_ENTRY_INDEX_BC[ChildValueFirst], childValueFirst);
            constexpr int left_most = SUB_ENTRY_INDEX_BC[ChildValueFirst];
            int mask = 1 << (left_most - 1);
            if (childValueFirst & mask) {
                for (int i = SUB_ENTRY_INDEX_BC[ChildValueSecond]; i < 32; ++i) {
                    mask <<= 1;
                    childValueFirst |= mask;
                }
            }
        }

        void set_childValueSecond(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, SUB_ENTRY_INDEX_BC[ChildValueSecond], childValueSecond);
            constexpr int left_most = SUB_ENTRY_INDEX_BC[ChildValueSecond];
            int mask = 1 << (left_most - 1);
            if (childValueSecond & mask) {
                for (int i = SUB_ENTRY_INDEX_BC[ChildValueSecond]; i < 32; ++i) {
                    mask <<= 1;
                    childValueSecond |= mask;
                }
            }
        }

        void set_childDuration(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, SUB_ENTRY_INDEX_BC[ChildDuration], childDuration);
        }

        void set_subEntry(const std::deque<bool> &frame_bool, const int start) {
            int offset = start;
            set_childId(frame_bool, offset);
            offset += SUB_ENTRY_INDEX_BC[ChildId];
            set_childEventTime(frame_bool, offset);
            offset += SUB_ENTRY_INDEX_BC[ChildEventTime];
            set_childValueFirst(frame_bool, offset);
            offset += SUB_ENTRY_INDEX_BC[ChildValueFirst];
            set_childValueSecond(frame_bool, offset);
            offset += SUB_ENTRY_INDEX_BC[ChildValueSecond];
            set_childDuration(frame_bool, offset);
            //             offset += SUB_ENTRY_INDEX_BC[ChildDuration];
        }

        [[nodiscard]] std::string to_string() const {
            return "childId: " + std::to_string(childId) + '\n'
                   + "childEventTime\n"
                   + childEventTime.to_string()
                   + "childValueFirst: " + std::to_string(childValueFirst / 10.0) + '\n'
                   + "childValueSecond: " + std::to_string(childValueSecond / 10000.0) + '\n'
                   + "childDuration: " + std::to_string(childDuration) + '\n';
        }
    };

public:
    struct Entry {
        int parentId;
        Time parentEventTime;
        int parentStatus;
        SubEntry subEntries[3];

        enum EntryIndex {
            ParentId, ParentEventTime, ParentStatus, ChildFirst, ChildSecond, ChildThird
        };

        static constexpr int ENTRY_INDEX_BC[] = {
            8, 33, 2, 74, 74, 74
        };

        static void set_int(const std::deque<bool> &frame_bool, const int start, const int bc, int &val) {
            int offset = start;
            int mask = 1 << (bc - 1);
            for (int i = 0; i < bc; ++i) {
                if (frame_bool[offset]) {
                    val |= mask;
                }
                ++offset;
                mask >>= 1;
            }
        }

        void set_parentId(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, ENTRY_INDEX_BC[ParentId], parentId);
        }

        void set_parentEventTime(const std::deque<bool> &frame_bool, const int start) {
            parentEventTime.set_time(frame_bool, start);
        }

        void set_parentStatus(const std::deque<bool> &frame_bool, const int start) {
            set_int(frame_bool, start, ENTRY_INDEX_BC[ParentStatus], parentStatus);
        }

        void set_subEntries(const std::deque<bool> &frame_bool, const int start) {
            int offset = start;
            subEntries[0].set_subEntry(frame_bool, offset);
            offset += ENTRY_INDEX_BC[ChildFirst];
            subEntries[1].set_subEntry(frame_bool, offset);
            offset += ENTRY_INDEX_BC[ChildSecond];
            subEntries[2].set_subEntry(frame_bool, offset);
            // offset += ENTRY_INDEX_BC[ChildThird];
        }

        void set_entry(const std::deque<bool> &frame_bool, const int start) {
            int offset = start;
            set_parentId(frame_bool, offset);
            offset += ENTRY_INDEX_BC[ParentId];
            set_parentEventTime(frame_bool, offset);
            offset += ENTRY_INDEX_BC[ParentEventTime];
            set_parentStatus(frame_bool, offset);
            offset += ENTRY_INDEX_BC[ParentStatus];
            set_subEntries(frame_bool, offset);
            // offset += ENTRY_INDEX_BC[ChildFirst];
            // offset += ENTRY_INDEX_BC[ChildSecond];
            // offset += ENTRY_INDEX_BC[ChildThird];
        }

        [[nodiscard]] std::string to_string() const {
            return "parentId: " + std::to_string(parentId) + '\n'
                   + "parentEventTime:\n"
                   + parentEventTime.to_string()
                   + "parentStatus: " + std::to_string(parentStatus) + '\n'
                   + "subEntries[0]\n"
                   + subEntries[0].to_string()
                   + "subEntries[1]\n"
                   + subEntries[1].to_string()
                   + "subEntries[2]\n"
                   + subEntries[2].to_string();
        }
    };

private:
    // preamble (8) + reserved (6) + length(10)
    static constexpr int HEAD_PREAMBLE_BIT_COUNT = 8;
    static constexpr int HEAD_RESERVED_BIT_COUNT = 6;
    static constexpr int HEAD_LENGTH_BIT_COUNT = 10;
    static constexpr int HEAD_BIT_COUNT = HEAD_PREAMBLE_BIT_COUNT + HEAD_RESERVED_BIT_COUNT + HEAD_LENGTH_BIT_COUNT;

    // type (12) + 265 * n + pad
    static constexpr int BODY_TYPE_BIT_COUNT = 12;
    static constexpr int BODY_ENTRY_BIT_COUNT = 265;

    // crc (24)
    static constexpr int TAIL_BIT_COUNT = 24;


    static constexpr int FRAME_BIT_COUNT_BOUND = 1002 * 8;
    std::deque<bool> frame_bool;
    int idx = 0;

    int head_length = 0;
    int body_type = 0;
    int body_entity_cnt = 0;
    int body_pad_cnt = 0;
    std::vector<Entry> body_entities;

    void fill_frame_bool(const char *frame, const int frame_size) {
        int offset = 0;
        frame_bool.clear();
        frame_bool.resize(frame_size * 8, false);
        assert(frame_bool.size() <= FRAME_BIT_COUNT_BOUND);
        for (int i = 0; i < frame_size; ++i) {
            int bit_mask = 1 << (8 - 1);
            for (int j = 0; j < 8; ++j) {
                if (frame[i] & bit_mask) {
                    frame_bool[offset] = true;
                }
                ++offset;
                bit_mask >>= 1;
            }
        }
    }

    void set_int(const int start, const int bc, int &val) const {
        int offset = start;
        int mask = 1 << (bc - 1);
        for (int i = 0; i < bc; ++i) {
            if (frame_bool[offset]) {
                val |= mask;
            }
            ++offset;
            mask >>= 1;
        }
    }

    void set_head_length() {
        set_int(idx, HEAD_LENGTH_BIT_COUNT, head_length);
    }

    void set_body_type() {
        set_int(idx, BODY_TYPE_BIT_COUNT, body_type);
    }

    void set_body_entity_cnt(const int frame_size) {
        int body_bit_count = frame_size * 8;
        body_bit_count -= HEAD_BIT_COUNT;
        body_bit_count -= TAIL_BIT_COUNT;
        body_entity_cnt = (body_bit_count - BODY_TYPE_BIT_COUNT) / BODY_ENTRY_BIT_COUNT;
    }

    void set_body_pad_cnt(const int frame_size) {
        // call after set_body_entity_cnt
        int body_bit_count = frame_size * 8;
        body_bit_count -= HEAD_BIT_COUNT;
        body_bit_count -= TAIL_BIT_COUNT;
        body_pad_cnt = body_bit_count - BODY_TYPE_BIT_COUNT - body_entity_cnt * BODY_ENTRY_BIT_COUNT;
    }

    void set_body_entities() {
        // call after set_body_entity_cnt
        int offset = idx;
        for (int i = 0; i < body_entity_cnt; ++i) {
            Entry entry{};
            entry.set_entry(frame_bool, offset);
            body_entities.emplace_back(entry);
            offset += BODY_ENTRY_BIT_COUNT;
        }
    }

    [[nodiscard]] static bool verify_tail_crc(const char *frame, const int frame_size) {
        return CRC24Q::crc24q_check(reinterpret_cast<const unsigned char *>(frame), frame_size);
    }

    [[nodiscard]] static bool verify_head_preamble(const char *frame, const int frame_size) {
        constexpr unsigned char preamble = 0b11010011;
        return static_cast<unsigned char>(frame[0]) == preamble;
    }

public:
    std::vector<Entry> transform(const char *frame, const int frame_size) {
        idx = 0;
        fill_frame_bool(frame, frame_size);
        if (!verify_tail_crc(frame, frame_size)) {
            return {};
        }
        // head part
        if (!verify_head_preamble(frame, frame_size)) {
            return {};
        }
        idx += HEAD_PREAMBLE_BIT_COUNT;
        idx += HEAD_RESERVED_BIT_COUNT;
        set_head_length();
        idx += HEAD_LENGTH_BIT_COUNT;
        // body part
        set_body_type();
        idx += BODY_TYPE_BIT_COUNT;
        set_body_entity_cnt(frame_size);
        set_body_pad_cnt(frame_size);
        set_body_entities();
        // tail part
        // nothing
        return body_entities;
    }
};

#endif //DESERIALIZER_H
