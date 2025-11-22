# CWars - AI Coding Assistant Instructions

## Project Overview
CWars is a multiplayer RTS game where players **write code to control armies** in a persistent, shared world. Players interact exclusively through **REST APIs** (no traditional frontend) to manage settlements, units, and resources in an open free-for-all environment.

**Key Concept**: This is a programmable RTS - players write scripts/bots that call APIs to control their civilization, not a traditional click-to-play game.

## Architecture (Planned)
- **Backend**: Python-based REST API servers (multiple instances behind load balancer)
- **No Traditional Frontend**: All interaction via API calls - players write client code/bots
- **Network Design**: See `design/networking/cwars_network_architecture.drawio` for multi-server architecture
- **Game State**: Persistent shared world with continuous simulation

## Core Game Systems (Design Phase)

### Resource Economy Chain
Resources flow through production chains (see `design/game/Resources.md`):
- Raw materials → Logs → Planks (construction)
- Grain → Flour → Bread (basic food)
- Grain → Pigs → Meat + Skins → Leather (advanced food + armor)
- Coal + Ore → Metal Bars → Weapons/Equipment

**Critical**: Every production step requires specific buildings and worker units.

### Unit Management (`design/game/Units.md`)
- **Worker Types**: Each has specialized role (WoodCutter, Farmer, Miller, Blacksmith, etc.)
- **Military Units**: Axe Fighters, Spearmen, Bowmen, Scouts (mounted)
- **Special Units**: Caravan (spawns new settlements), Servant (logistics), Builder (construction)

**Key Mechanic**: Units require continuous food supply and have aging/death mechanics.

### Building System (`design/game/Buildings.md`)
Buildings enable production chains:
- Resource gathering: Woodcutter's Hut, Farm, Coal/Gold Mines
- Processing: Lumber Mill, Mill, Bakery, Tannery, Gold Foundry
- Military: Barracks (training), Workshop (equipment production), Stable (horses)
- Support: Storage, School (worker training), Tavern (worker feeding)

### Survival Challenges (`design/game/GameLoop.md`)
Three core pressures shape gameplay:
1. **Hunger**: Units die without regular feeding → maintain food production chain
2. **Aging**: Units die of old age → continuously train replacements
3. **Scarcity**: Limited coal/iron/gold veins → force exploration and expansion

## Development Conventions

### Design-First Approach
This codebase is **currently in design phase**. All game mechanics are documented in `design/` before implementation:
- Always reference design docs when implementing features
- Game mechanics in `design/game/*.md` define the complete rule system
- API contracts will be defined in `design/networking/apis.MD` (currently minimal)

### Anticipated Stack
Based on `.gitignore`:
- **Language**: Python (PyPI packaging, pytest expected)
- **Expected Tools**: Poetry/PDM for dependency management
- **Testing**: Likely pytest with coverage reports

### When Implementing Code
- Follow the exact mechanics defined in design docs - don't improvise game balance
- Unit stats (`design/game/Stats.md`) are authoritative - preserve all attributes
- Production chains must match resource flow in `design/game/Resources.md`
- Building prerequisites and outputs must match `design/game/Buildings.md`

## Key Files Reference
- `design/game/GameLoop.md` - Player lifecycle: spawn → expand → end conditions
- `design/game/Units.md` - All unit types with roles and purpose
- `design/game/Buildings.md` - Complete building list with production roles
- `design/game/Resources.md` - Resource types and production chains
- `design/game/Stats.md` - Unit attribute system (health, age, food, combat stats)
- `design/networking/apis.MD` - REST API specifications (under development)

## Important Design Decisions
1. **No Visual Client**: Players must write code to interact with the game - design APIs for programmatic consumption
2. **Persistent World**: Game state survives between sessions - design for long-running processes
3. **Multiplayer from Start**: All mechanics assume shared world with multiple competing players
4. **Code-Driven Gameplay**: API design is the UX - prioritize clear, discoverable endpoints

When implementing, ensure APIs are bot-friendly with clear state queries, batch operations, and efficient polling/webhooks for updates.
