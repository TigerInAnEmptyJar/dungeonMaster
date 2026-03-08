#pragma once

#include <memory>
#include <vector>

#include <QObject>

namespace infrastructure {

/**
 * \brief A tree node that owns its children via shared_ptr and holds a weak_ptr to its parent.
 *
 * Structural changes (insert/remove child) are announced via two families of signals:
 *  - Local signals  – fire only on the immediate parent node.
 *  - Subtree signals – also forwarded from each child, so a single connection to
 *                      any ancestor receives events from the entire subtree below it.
 *
 * \note Objects must be managed by std::shared_ptr (e.g. std::make_shared<TreeItem>())
 *       so that weak_from_this() is functional.
 */
class TreeItem : public QObject, public std::enable_shared_from_this<TreeItem>
{
  Q_OBJECT

public:
  explicit TreeItem(QObject* parent = nullptr);
  ~TreeItem();

  TreeItem(TreeItem const&) = delete;
  auto operator=(TreeItem const&) -> TreeItem& = delete;
  TreeItem(TreeItem&&) = delete;
  auto operator=(TreeItem&&) -> TreeItem& = delete;

  /**
   * \brief Inserts \p child into this node's children at position \p index.
   *
   * If \p index is greater than or equal to the current child count, the item
   * is appended. Sets the child's parent to this node and connects the child's
   * subtree signals for upward propagation.
   * Emits childAboutToBeInserted / childInserted and the corresponding subtree signals.
   *
   * \param index  Desired 0-based insertion position.
   * \param child  The child node to insert.
   */
  auto insertChild(int index, std::shared_ptr<TreeItem> child) -> void;

  /**
   * \brief Removes \p child from this node's children.
   *
   * Clears the child's parent and disconnects its subtree signal forwarding.
   * Emits childAboutToBeRemoved / childRemoved (with the former index) and the
   * corresponding subtree signals. Does nothing if \p child is not a direct
   * child of this node.
   *
   * \param child  The child node to remove.
   */
  auto removeChild(std::shared_ptr<TreeItem> child) -> void;

  /**
   * \brief Returns a weak_ptr to the parent node, or an empty weak_ptr for the root.
   */
  auto parentItem() const -> std::weak_ptr<TreeItem>;

  /**
   * \brief Returns the number of direct children.
   */
  auto size() const -> int;

  /**
   * \brief Returns the child at 0-based \p index.
   *
   * \pre \p index must be in [0, childCount()).
   */
  auto childAt(int index) const -> std::shared_ptr<TreeItem>;

  /// Iterator type for range-based iteration over direct children.
  using ChildIterator = std::vector<std::shared_ptr<TreeItem>>::const_iterator;

  /**
   * \brief Returns an iterator to the first child.
   */
  auto begin() const -> ChildIterator;

  /**
   * \brief Returns a past-the-end iterator for the children.
   */
  auto end() const -> ChildIterator;

Q_SIGNALS:
  // ── Local signals (direct parent only) ───────────────────────────────────
  void childAboutToBeInserted(int index, std::shared_ptr<TreeItem> child);
  void childInserted(int index, std::shared_ptr<TreeItem> child);
  void childAboutToBeRemoved(int index, std::shared_ptr<TreeItem> child);
  void childRemoved(int index, std::shared_ptr<TreeItem> child);

  // ── Subtree signals (propagated upward through ancestors) ─────────────────
  void subtreeChildAboutToBeInserted(std::shared_ptr<TreeItem> parent, int index,
                                     std::shared_ptr<TreeItem> child);
  void subtreeChildInserted(std::shared_ptr<TreeItem> parent, int index,
                            std::shared_ptr<TreeItem> child);
  void subtreeChildAboutToBeRemoved(std::shared_ptr<TreeItem> parent, int index,
                                    std::shared_ptr<TreeItem> child);
  void subtreeChildRemoved(std::shared_ptr<TreeItem> parent, int index,
                           std::shared_ptr<TreeItem> child);

private:
  struct Impl;
  std::unique_ptr<Impl> _p;
};

} // namespace infrastructure
