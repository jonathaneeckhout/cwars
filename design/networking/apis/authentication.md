# Authentication

All authenticated endpoints require a bearer token in the Authorization header:

```
Authorization: Bearer <token>
```

Tokens are obtained via the login endpoint and expire after 24 hours.

---

## Authentication Endpoints

### POST /auth/register
Create a new player account.

**Request:**
```json
{
  "username": "string (3-20 chars, alphanumeric + underscore)",
  "email": "string (valid email)",
  "password": "string (min 8 chars)"
}
```

**Response:** `201 Created`
```json
{
  "player_id": "uuid",
  "username": "string",
  "created_at": "timestamp"
}
```

### POST /auth/login
Authenticate and receive access token.

**Request:**
```json
{
  "username": "string",
  "password": "string"
}
```

**Response:** `200 OK`
```json
{
  "token": "jwt_token_string",
  "expires_at": "timestamp",
  "player_id": "uuid"
}
```

### POST /auth/logout
Invalidate current token.

**Response:** `204 No Content`

---