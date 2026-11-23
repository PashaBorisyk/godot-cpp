/**************************************************************************/
/*  pair.hpp                                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <godot_cpp/core/defs.hpp>

namespace godot {

/**
 * Stores two values. Ordering compares the first member, then the second for ties.
 *
 * **Used for** keeping an atlas frame's row and column together.
 *
 * - `F` — First member type.
 * - `S` — Second member type.
 *
 * ```cpp
 * // Keep the row and column of the selected frame together.
 * Pair<int, int> frame(atlas_row, atlas_column);
 * Rect2i region(frame.second * tile_width, frame.first * tile_height, tile_width, tile_height);
 * ```
 */
template <typename F, typename S>
struct Pair {
	/**
	 * Stores the first value and primary sort key.
	 *
	 * **Used for** holding the row of an atlas frame.
	 *
	 * ```cpp
	 * // frame is a Pair<int, int> holding an atlas row and column.
	 * int source_y = frame.first * tile_height;
	 * ```
	 */
	F first{};
	/**
	 * Stores the second value and breaks sort ties.
	 *
	 * **Used for** holding the column of an atlas frame.
	 *
	 * ```cpp
	 * // frame is a Pair<int, int> holding an atlas row and column.
	 * int source_x = frame.second * tile_width;
	 * ```
	 */
	S second{};

	/**
	 * Initializes both values with empty braces. Scalars start at zero.
	 *
	 * **Used for** starting atlas traversal at row and column zero.
	 *
	 * ```cpp
	 * Pair<int, int> cell; // Start scanning an atlas at row 0, column 0.
	 * ++cell.second; // Advance to the next column.
	 * ```
	 */
	constexpr Pair() = default;
	/**
	 * Copies the two values into their members.
	 *
	 * **Used for** keeping an image's decoded width and height together.
	 *
	 * - `p_first` — Value to copy into `first`.
	 * - `p_second` — Value to copy into `second`.
	 *
	 * ```cpp
	 * // Keep both dimensions returned by the image decoder.
	 * Pair<int, int> dimensions(decoded_width, decoded_height);
	 * bool needs_downsample = dimensions.first > max_texture_width;
	 * ```
	 */
	constexpr Pair(const F &p_first, const S &p_second) :
			first(p_first), second(p_second) {}

	/**
	 * Returns `true` if both members are equal.
	 *
	 * **Used for** reusing a cached draw command
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * bool reuse_command = current == cached;
	 * ```
	 */
	constexpr bool operator==(const Pair &p_other) const { return first == p_other.first && second == p_other.second; }
	/**
	 * Returns `true` if either member differs.
	 *
	 * **Used for** rebuilding a draw command when its state changes
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * bool rebuild_command = current != cached;
	 * ```
	 */
	constexpr bool operator!=(const Pair &p_other) const { return first != p_other.first || second != p_other.second; }
	/**
	 * Returns `true` if this entry precedes the other.
	 * Compares `first`, then `second` when tied.
	 *
	 * **Used for** choosing which entry to draw first
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * const auto &first_to_draw = current < candidate ? current : candidate;
	 * ```
	 */
	constexpr bool operator<(const Pair &p_other) const { return first == p_other.first ? (second < p_other.second) : (first < p_other.first); }
	/**
	 * Returns `true` if this entry precedes or equals the other.
	 * Compares `first`, then `second` when tied.
	 *
	 * **Used for** scanning through an inclusive upper bound
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * bool within_scan_range = current <= last;
	 * ```
	 */
	constexpr bool operator<=(const Pair &p_other) const { return first == p_other.first ? (second <= p_other.second) : (first < p_other.first); }
	/**
	 * Returns `true` if this entry follows the other.
	 * Compares `first`, then `second` when tied.
	 *
	 * **Used for** choosing the first entry in a reverse scan
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * const auto &first_in_reverse = current > candidate ? current : candidate;
	 * ```
	 */
	constexpr bool operator>(const Pair &p_other) const { return first == p_other.first ? (second > p_other.second) : (first > p_other.first); }
	/**
	 * Returns `true` if this entry follows or equals the other.
	 * Compares `first`, then `second` when tied.
	 *
	 * **Used for** scanning backward to an inclusive lower bound
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (texture page, frame index) entries from the draw list.
	 * bool within_reverse_scan = current >= first;
	 * ```
	 */
	constexpr bool operator>=(const Pair &p_other) const { return first == p_other.first ? (second >= p_other.second) : (first > p_other.first); }
};

/**
 * Orders pairs by first member, then second member.
 *
 * **Used for** sorting an atlas draw list by texture page and frame index.
 *
 * - `F` — First member type.
 * - `S` — Second member type.
 *
 * ```cpp
 * // Sort existing frame entries by texture page, then by frame index.
 * std::sort(frames.begin(), frames.end(), PairSort<int, int>());
 * ```
 */
template <typename F, typename S>
struct PairSort {
	/**
	 * Returns `true` when the left pair precedes the right pair.
	 *
	 * **Used for** selecting the next frame while merging two sorted render lists.
	 *
	 * - `p_lhs` — Left pair.
	 * - `p_rhs` — Right pair.
	 *
	 * ```cpp
	 * // Merge two sorted lists of (texture page, frame index) pairs.
	 * const auto &next_frame = PairSort<int, int>()(left_frame, right_frame) ? left_frame : right_frame;
	 * ```
	 */
	constexpr bool operator()(const Pair<F, S> &p_lhs, const Pair<F, S> &p_rhs) const {
		return p_lhs < p_rhs;
	}
};

/**
 * Marks a pair zero-constructible if both member types are zero-constructible.
 *
 * **Used for** letting array storage zero-initialize newly allocated atlas counters.
 *
 * - `F` — First member type tested by the trait.
 * - `S` — Second member type tested by the trait.
 *
 * ```cpp
 * // In the atlas importer, new row/column counters begin at zero.
 * LocalVector<Pair<int, int>> counters;
 * counters.resize(page_count);
 * ```
 */
template <typename F, typename S>
struct is_zero_constructible<Pair<F, S>> : std::conjunction<is_zero_constructible<F>, is_zero_constructible<S>> {};

/**
 * Stores a const key and a mutable value.
 *
 * **Used for** editing a map entry without changing its lookup key.
 *
 * - `K` — Key type.
 * - `V` — Value type.
 *
 * ```cpp
 * // Store a tile ID and its collision-layer mask, then enable layer 2.
 * KeyValue<int, uint32_t> entry(tile_id, collision_mask);
 * entry.value |= uint32_t(1) << 1;
 * ```
 */
template <typename K, typename V>
struct KeyValue {
	/**
	 * Stores the read-only key.
	 *
	 * **Used for** identifying the entry in map lookups.
	 *
	 * ```cpp
	 * // Copy the cache entry's ID into a diagnostics record.
	 * int preview_id = entry.key;
	 * ```
	 */
	const K key{};
	/**
	 * Stores the mutable value for this key.
	 *
	 * **Used for** updating a tile's collision mask while keeping its ID.
	 *
	 * ```cpp
	 * // Change the layer mask without replacing the entry's tile ID.
	 * entry.value |= uint32_t(1) << 1;
	 * ```
	 */
	V value{};

	/**
	 * Deletes copy assignment because the key is const.
	 *
	 * **Used for** enforcing fixed keys when editing copied data.
	 *
	 * - `p_kv` — Source entry of the deleted assignment.
	 *
	 * ```cpp
	 * // Whole-entry copy assignment is deleted. Copy the preview state into a stable entry.
	 * destination.value = source.value;
	 * ```
	 */
	KeyValue &operator=(const KeyValue &p_kv) = delete;
	/**
	 * Deletes move assignment because the key is const.
	 *
	 * **Used for** requiring callers to move values into existing entries.
	 *
	 * - `p_kv` — Source entry of the deleted move assignment.
	 *
	 * ```cpp
	 * // Whole-entry move assignment is deleted. Move the label into a stable entry.
	 * destination.value = std::move(source.value);
	 * ```
	 */
	KeyValue &operator=(KeyValue &&p_kv) = delete;

	/**
	 * Copies the source key and value.
	 *
	 * **Used for** saving an entry's state before editing its value.
	 *
	 * - `p_kv` — Entry whose key and value are copied.
	 *
	 * ```cpp
	 * // Capture the old (tile ID, collision mask) before editing it.
	 * KeyValue<int, uint32_t> before(entry);
	 * entry.value = updated_mask;
	 * ```
	 */
	constexpr KeyValue(const KeyValue &p_kv) = default;
	/**
	 * Initializes the key from a const rvalue and the value from an rvalue.
	 *
	 * **Used for** transferring a temporary value into an entry with its original key.
	 *
	 * - `p_kv` — Source entry. Its value can be moved.
	 *
	 * ```cpp
	 * // Transfer a temporary preview label into an entry whose ID stays fixed.
	 * KeyValue<int, String> preview(std::move(pending_label));
	 * ```
	 */
	constexpr KeyValue(KeyValue &&p_kv) = default;
	/**
	 * Copies the supplied key and value into the entry.
	 *
	 * **Used for** associating a preview ID with a title.
	 *
	 * - `p_key` — Key to copy.
	 * - `p_value` — Value to copy.
	 *
	 * ```cpp
	 * // Associate a preview ID with the title loaded from its metadata.
	 * KeyValue<int, String> entry(preview_id, title);
	 * ```
	 */
	constexpr KeyValue(const K &p_key, const V &p_value) :
			key(p_key), value(p_value) {}
	/**
	 * Copies the pair's first member as the key and its second as the value.
	 *
	 * **Used for** using a tile ID and mask imported as a pair.
	 *
	 * - `p_pair` — Pair whose members supply the key and value.
	 *
	 * ```cpp
	 * // An importer supplied (tile ID, layer mask) as a Pair<int, uint32_t>.
	 * KeyValue<int, uint32_t> entry(imported_tile);
	 * ```
	 */
	constexpr KeyValue(const Pair<K, V> &p_pair) :
			key(p_pair.first), value(p_pair.second) {}

	/**
	 * Returns `true` if both members are equal.
	 *
	 * **Used for** reusing a cached draw command
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * bool reuse_command = current == cached;
	 * ```
	 */
	constexpr bool operator==(const KeyValue &p_other) const { return key == p_other.key && value == p_other.value; }
	/**
	 * Returns `true` if either member differs.
	 *
	 * **Used for** rebuilding a draw command when its state changes
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * bool rebuild_command = current != cached;
	 * ```
	 */
	constexpr bool operator!=(const KeyValue &p_other) const { return key != p_other.key || value != p_other.value; }
	/**
	 * Returns `true` if this entry precedes the other.
	 * Compares `key`, then `value` when tied.
	 *
	 * **Used for** choosing which entry to draw first
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * const auto &first_to_draw = current < candidate ? current : candidate;
	 * ```
	 */
	constexpr bool operator<(const KeyValue &p_other) const { return key == p_other.key ? (value < p_other.value) : (key < p_other.key); }
	/**
	 * Returns `true` if this entry precedes or equals the other.
	 * Compares `key`, then `value` when tied.
	 *
	 * **Used for** scanning through an inclusive upper bound
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * bool within_scan_range = current <= last;
	 * ```
	 */
	constexpr bool operator<=(const KeyValue &p_other) const { return key == p_other.key ? (value <= p_other.value) : (key < p_other.key); }
	/**
	 * Returns `true` if this entry follows the other.
	 * Compares `key`, then `value` when tied.
	 *
	 * **Used for** choosing the first entry in a reverse scan
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * const auto &first_in_reverse = current > candidate ? current : candidate;
	 * ```
	 */
	constexpr bool operator>(const KeyValue &p_other) const { return key == p_other.key ? (value > p_other.value) : (key > p_other.key); }
	/**
	 * Returns `true` if this entry follows or equals the other.
	 * Compares `key`, then `value` when tied.
	 *
	 * **Used for** scanning backward to an inclusive lower bound
	 *
	 * - `p_other` — Right-hand entry to compare.
	 *
	 * ```cpp
	 * // Compare (tile ID, collision mask) entries from the draw list.
	 * bool within_reverse_scan = current >= first;
	 * ```
	 */
	constexpr bool operator>=(const KeyValue &p_other) const { return key == p_other.key ? (value >= p_other.value) : (key > p_other.key); }
};

/**
 * Orders entries by their keys alone.
 *
 * **Used for** finding an entry by key with a binary search.
 *
 * - `K` — Key type compared by the sorter.
 * - `V` — Value type ignored by the sorter.
 *
 * ```cpp
 * // Binary-search sorted entries by tile ID with a probe whose mask is ignored.
 * auto pos = std::lower_bound(first, last, KeyValue<int, uint32_t>(tile_id, 0), KeyValueSort<int, uint32_t>());
 * ```
 */
template <typename K, typename V>
struct KeyValueSort {
	/**
	 * Returns `true` if the left key is smaller than the right key.
	 *
	 * **Used for** merging registries sorted by tile ID.
	 *
	 * - `p_lhs` — Entry that supplies the left key.
	 * - `p_rhs` — Entry that supplies the right key.
	 *
	 * ```cpp
	 * // In a merge of two tile registries, select the next entry by ID alone.
	 * const auto &next = KeyValueSort<int, uint32_t>()(left_entry, right_entry) ? left_entry : right_entry;
	 * ```
	 */
	constexpr bool operator()(const KeyValue<K, V> &p_lhs, const KeyValue<K, V> &p_rhs) const {
		return p_lhs.key < p_rhs.key;
	}
};

/**
 * Marks an entry zero-constructible if both member types are zero-constructible.
 *
 * **Used for** checking whether array storage can clear newly allocated entries.
 *
 * - `K` — Key type tested by the trait.
 * - `V` — Value type tested by the trait.
 *
 * ```cpp
 * // Verify that array placement can clear newly allocated tile ID and mask slots to zero.
 * static_assert(is_zero_constructible_v<KeyValue<int, uint32_t>>);
 * ```
 */
template <typename K, typename V>
struct is_zero_constructible<KeyValue<K, V>> : std::conjunction<is_zero_constructible<K>, is_zero_constructible<V>> {};

} // namespace godot
