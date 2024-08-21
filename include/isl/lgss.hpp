#ifndef ISL_PROJECT_GSS_HPP
#define ISL_PROJECT_GSS_HPP

#include <isl/io.hpp>
#include <isl/memory.hpp>

namespace isl
{
    template<typename T>
    class GSS
    {
    public:
        struct Node;
        using GSSNode = std::shared_ptr<Node>;

        struct Node
        {
            std::vector<GSSNode> previous;
            std::shared_ptr<T> value;
            SmallId slot;
            SmallId inputPosition;

            template<typename... Ts>
            explicit Node(SmallId grammar_slot, SmallId input_position, Ts &&...args)
              : value{makeShared<T>(std::forward<Ts>(args)...)}
              , slot{grammar_slot}
              , inputPosition{input_position}
            {}

            auto addPrevious(GSSNode node) -> void
            {
                auto it = std::ranges::find(previous, node);

                if (it == previous.end()) {
                    previous.emplace_back(std::move(node));
                }
            }
        };

        struct Descriptor
        {
            GSSNode stack;
            SmallId slot;
            SmallId inputLocation;

            auto
                operator<=>(const Descriptor &other) const noexcept -> std::weak_ordering = default;
        };

    private:
        std::vector<std::vector<GSSNode>> levels;
        std::set<Descriptor> descriptors;

    public:
        template<typename... Ts>
        auto push(GSSNode parent, SmallId input_position, SmallId slot, Ts &&...args)
            -> std::pair<GSSNode, bool>
        {
            if (input_position >= levels.size()) {
                levels.resize(input_position + 1);
            }

            auto node = findAtLevel(input_position, slot);
            auto inserted = (node == nullptr);

            if (node == nullptr) {
                node = makeShared<Node>(input_position, slot, std::forward<Ts>(args)...);
            }

            node->addPrevious(std::move(parent));
            return {std::move(node), inserted};
        }

        auto pop(GSSNode node) -> void
        {
            if (node->slot != 0)
            {
                for (auto &prev: node->previous)
                {
                    add(node->slot, prev, )
                }
            }
        }

        auto add(SmallId slot, SmallId input_position, GSSNode node) -> void
        {
            descriptors.emplace(std::move(node), slot, input_position);
        }

    private:
        auto findAtLevel(SmallId input_position, SmallId slot) const -> GSSNode
        {
            const auto &level = levels.at(input_position);

            auto it = std::ranges::find_if(level, [slot](const GSSNode &elem) {
                return elem->slot == slot;
            });

            if (it == level.end()) {
                return nullptr;
            }

            return *it;
        }
    };

    template<typename T>
    using LGSSNode = typename LGSStack<T>::GSSNode;
}// namespace isl

#endif /* ISL_PROJECT_GSS_HPP */
