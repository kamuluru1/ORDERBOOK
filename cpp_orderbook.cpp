#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

class Orderbook {
    public:

        enum class OrderType { Market, Limit, GoodTillCancelled, FillOrKill_Limit };
        enum class Side { Buy, Sell };
        
        class Order {
            public:
                // Constructor
                Order(int id, OrderType type, Side side, double price, int quantity)
                    : id(id), type(type), side(side), price(price), quantity(quantity) {}
                
                // Getters
                int getId() const { return id; }
                OrderType getType() const { return type; }
                Side getSide() const { return side; }
                double getPrice() const { return price; }
                int getQuantity() const { return quantity; }
                
                // Method to update order quantity
                void setQuantity(int new_quantity) { quantity = new_quantity; }

            private:
                int id;
                OrderType type;
                Side side;
                double price;
                int quantity;
        };

        // Method to add order to orderbook
        void addOrder(const Order& order) {
            orders.push_back(order);
        }

        // Method to cancel order by using the order id
        void cancelOrder(int orderId) {
            auto it = std::remove_if(orders.begin(), orders.end(), [orderId](const Order& order) {
                return order.getId() == orderId;
            });

            if (it != orders.end()) {
                std::cout << "Order " << orderId << " has been cancelled." << std::endl;
                orders.erase(it, orders.end());
            }
        }

        // Method to match orders
        void matchOrders() {
            // Market orders
            for(auto it = orders.begin(); it != orders.end();) {
                if(it->getType() == OrderType::Market) {
                    auto matchIt = findMatch(it, it->getQuantity());
                    if(matchIt != orders.end()) {
                        executeOrder(it, matchIt);
                        it = orders.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            }

            // GTC orders
            for(auto it = orders.begin(); it != orders.end();) {
                if(it->getType() == OrderType::GoodTillCancelled) {
                    auto matchIt = findMatch(it, it->getQuantity());
                    if(matchIt != orders.end()) {
                        executeOrder(it, matchIt);
                        it = orders.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
            

        }

        // Print all orders in the orderbook
        void printOrders() const {
            for(const auto& order : orders) {
                printOrder(order);
            }
        }

    private:
        std::vector<Order> orders; // Vector to store orders

        // Method to find a match for the order
        std::vector<Order>::iterator findMatch(std::vector<Order>::iterator orderIt, int quantity, bool fullMatch = false) {
            for (auto it = orders.begin(); it != orders.end(); ++it) {
                if (it->getSide() != orderIt->getSide() &&
                    ((orderIt->getSide() == Side::Buy && it->getPrice() <= orderIt->getPrice()) ||
                    (orderIt->getSide() == Side::Sell && it->getPrice() >= orderIt->getPrice())) &&
                    (!fullMatch || it->getQuantity() >= quantity)) {
                    return it;
                }
            }
            return orders.end();
        }

        // Method to execute the order
        void executeOrder(std::vector<Order>::iterator& orderIt, std::vector<Order>::iterator& matchIt) {
            double fillPrice = matchIt->getPrice();
            std::cout << "Matched OrderID: " << orderIt->getId() << " with OrderID: " << matchIt->getId() << " at price: " << std::fixed << std::setprecision(5) << fillPrice << std::endl;
            matchIt->setQuantity(matchIt->getQuantity() - orderIt->getQuantity());
            if(matchIt->getQuantity() == 0) {
                orders.erase(matchIt);
            }

        }

        // Method to print order details
        void printOrder(const Order& order) const {
            std::cout << "OrderId: " << order.getId()
                      << " Type: " << static_cast<int>(order.getType())
                      << " Side: " << static_cast<int>(order.getSide())
                      << " Price: " << std::fixed << std::setprecision(5) << order.getPrice()
                      << " Quantity: " << order.getQuantity() << std::endl;
        }
};

//int argc, const char * argv[]
int main() {
    Orderbook orderbook;

    Orderbook::Order order1(1, Orderbook::OrderType::Market, Orderbook::Side::Buy, 0, 10);
    Orderbook::Order order2(2, Orderbook::OrderType::Limit, Orderbook::Side::Buy, 101.0, 20);
    Orderbook::Order order3(3, Orderbook::OrderType::Limit, Orderbook::Side::Buy, 99.0, 5);
    Orderbook::Order order4(4, Orderbook::OrderType::Market, Orderbook::Side::Buy, 0, 15);
    Orderbook::Order order5(5, Orderbook::OrderType::GoodTillCancelled, Orderbook::Side::Buy, 102.00, 10);
    Orderbook::Order order6(6, Orderbook::OrderType::FillOrKill_Limit, Orderbook::Side::Buy, 100.0, 8);
    Orderbook::Order order7(7, Orderbook::OrderType::FillOrKill_Limit, Orderbook::Side::Buy, 99.0, 2);
    Orderbook::Order order8(8, Orderbook::OrderType::FillOrKill_Limit, Orderbook::Side::Buy, 101.0, 8);

    orderbook.addOrder(order1);
    orderbook.addOrder(order2);
    orderbook.addOrder(order3);
    orderbook.addOrder(order4);
    orderbook.addOrder(order5);
    orderbook.addOrder(order6);
    orderbook.addOrder(order7);
    orderbook.addOrder(order8);

    std::cout << "Orders before matching: " << std::endl;
    orderbook.printOrders();

    orderbook.matchOrders();

    std::cout << "Orders after matching: " << std::endl;

    return 0;
}