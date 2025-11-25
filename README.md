# CWars - A Programmable RTS Game

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**CWars** is an open-source multiplayer real-time strategy game where players **write code to control their civilizations**. Instead of clicking buttons, players interact with the game world entirely through **REST APIs**, making this a unique fusion of gaming, programming, and strategy.


## Project Vision

This project aims to create a fun, educational platform where developers can:
- **Learn** by building bots and automation scripts
- **Collaborate** on an open-source game engine
- **Compete** in a persistent multiplayer world
- **Experiment** with AI, strategy algorithms, and distributed systems

Whether you're learning Python, exploring game development, or just want to build a bot that conquers the world, CWars welcomes you!

## How CWars Works

### The Core Concept
Players don't control units with a mouse - they **write scripts** that call REST APIs to:
- Query the game state (where are my units? what resources do I have?)
- Issue commands (build a farm, train workers, attack enemy settlements)
- Automate complex strategies (resource management, military tactics, diplomacy)

The game world is **persistent and shared** - all players exist in the same world, competing for scarce resources, forming alliances, or waging war.

### Starting Your Journey
1. **Spawn**: Each player starts with a **Caravan** at a random location
2. **Deploy**: Find a good spot and deploy your Caravan to create your first settlement (1 Storage, 3 Servants, 3 Builders, 15 Gold)
3. **Build**: Construct buildings to gather resources and train specialized workers
4. **Expand**: Grow your economy, raise an army, and explore the world
5. **Survive**: Keep your units fed, replace aging workers, and defend against threats

Your civilization ends when all your units are destroyed - but you can always respawn and try a new strategy!

## Game Systems

### Resource Economy
CWars features interconnected production chains (see [`design/game/Resources.md`](design/game/Resources.md)):

```
Trees → Logs → Planks (Construction)
Grain → Flour → Bread (Basic Food)
Grain → Pigs → Meat + Skins → Leather (Advanced Food & Armor)
```

Every step requires the right building and specialized workers!

### Unit Types
See [`design/game/Units.md`](design/game/Units.md) for complete details:
- **Workers**: WoodCutter, Farmer, Miller, Baker, Blacksmith, Miner, etc.
- **Logistics**: Servant (transport goods), Builder (construct buildings)
- **Military**: Axe Fighter, Spearman, Bowman, Scout (mounted)
- **Special**: Caravan (spawns settlements)

**Critical Mechanic**: Units need food and age over time - you must continuously manage population!

### Buildings
From [`design/game/Buildings.md`](design/game/Buildings.md):
- **Resource Gathering**: Woodcutter's Hut, Farm, Coal Mine, Gold Mine
- **Processing**: Lumber Mill, Mill, Bakery, Tannery, Gold Foundry
- **Military**: Barracks, Workshop, Stable
- **Infrastructure**: Storage, School (train workers), Tavern (feed units)

### Survival Challenges
Three core pressures create strategic tension (see [`design/game/GameLoop.md`](design/game/GameLoop.md)):
1. **Hunger**: Units starve without food → maintain farms and bakeries
2. **Aging**: Units die of old age → train replacements continuously
3. **Scarcity**: Limited coal/iron/gold veins → expand or die

## Technical Architecture

### Network Architecture
CWars uses a distributed backend architecture designed for **clarity, scalability, and security** over raw performance. See [`design/networking/Architecture.md`](design/networking/Architecture.md) for complete details.

**Key Components**:
- **Load Balancer (Nginx)**: Distributes traffic across multiple API servers with health checks and SSL termination
- **API Servers (Flask)**: Stateless REST servers that horizontally scale to handle player load
- **Rate Limiter**: Token bucket algorithm preventing abuse (100 GET/min, 30 POST/min per player)
- **Game State Database**: Centralized world state with optimized queries
- **User Database**: Authentication and player account management

**Communication Patterns**:
- **REST API**: Player scripts send commands and query game state
- **WebSocket**: Real-time event notifications (combat, construction completion, etc.)
- **Batch Endpoints**: Reduce HTTP overhead for multiple operations

The architecture prioritizes script-driven gameplay - since players write code to control their civilizations, millisecond response times aren't critical. Instead, the design focuses on stable, predictable API contracts that make bot development straightforward.

### APIs
All player interaction happens through REST APIs - there is no traditional game client. See [`design/networking/apis/`](design/networking/apis/) for complete API specifications.

**Core API Modules**:
- [`api.md`](design/networking/apis/api.md) - Overview and general conventions
- [`authentication.md`](design/networking/apis/authentication.md) - Login, registration, token management
- [`world.md`](design/networking/apis/world.md) - Query world state, map data, player visibility
- [`units.md`](design/networking/apis/units.md) - Unit commands (move, attack, train, feed)
- [`buildings.md`](design/networking/apis/buildings.md) - Construction, production, upgrades
- [`batch.md`](design/networking/apis/batch.md) - Batch operations for efficiency

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.



