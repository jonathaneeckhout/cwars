# CWars - A Programmable RTS Game

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**CWars** is an open-source multiplayer real-time strategy game where players **write code to control their civilizations**. Instead of clicking buttons, players interact with the game world entirely through **REST APIs**, making this a unique fusion of gaming, programming, and strategy.

> **Game meets Code**: Build settlements, manage resources, train armies, and conquer territories - all through the code you write!

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
Coal + Iron Ore → Metal Bars → Weapons & Equipment
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

**Status**: Currently in design phase - no code yet!

### Planned Stack
- **Backend**: Python-based REST API servers (scalable multi-instance)
- **Database**: TBD (needs to support persistent world state)
- **Architecture**: Multiple API servers behind a load balancer
- **No Frontend**: Players write client code in any language

See [`design/networking/cwars_network_architecture.drawio`](design/networking/cwars_network_architecture.drawio) for the network design.

### API Design (In Progress)
See [`design/networking/apis.MD`](design/networking/apis.MD) - specifications are under development!

The API will be designed for **bot-friendly** interaction:
- Clear state query endpoints
- Batch operations for efficiency
- Webhooks or polling for updates
- RESTful resource design

## Contributing

**We're just getting started!** This project is in the **design phase**, and we need contributors to:

### Game Design
- Review and refine game mechanics in `design/game/*.md`
- Balance resource chains and unit costs
- Design engaging gameplay loops
- Define complete API contracts

### Development (Coming Soon)
- Implement the game engine in Python
- Build the REST API server
- Create database schema for game state
- Develop simulation/tick system
- Write unit tests

### Documentation
- Improve design documents
- Write API documentation
- Create tutorial content for new players
- Document bot development strategies

### Community
- Build example bots
- Test and provide feedback
- Report issues and suggest features
- Help others get started

## Getting Started (For Contributors)

Since we're in the design phase, the best way to get started is:

1. **Read the design docs**: Start with [`design/game/GameLoop.md`](design/game/GameLoop.md) to understand player experience
2. **Review the systems**: Check out Units, Buildings, Resources, and Stats docs
3. **Explore the architecture**: Look at [`design/networking/cwars_network_architecture.drawio`](design/networking/cwars_network_architecture.drawio)
4. **Join the conversation**: Open issues to discuss implementation approaches
5. **Start coding**: Once you understand the design, help implement it!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Learning Goals

This project is perfect for learning:
- **Game Development**: Real-time simulation, state management, multiplayer systems
- **API Design**: Building bot-friendly, RESTful interfaces
- **Python**: Backend development, async programming, testing
- **Distributed Systems**: Multi-server architecture, load balancing, persistence
- **Bot Programming**: AI strategies, resource optimization, pathfinding
- **Open Source Collaboration**: Contributing to a real project with others

## The Dream

Imagine a world where players compete by writing the best bots:
- Automated resource gathering and production
- AI-driven military strategies
- Diplomatic protocols between player bots
- Economic trading systems
- Emergent gameplay from bot interactions

**Let's build this together!**

---

