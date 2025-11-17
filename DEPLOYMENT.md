# Guía de Deployment - Y Qué Comemos

Esta guía cubre cómo desplegar la aplicación en producción.

## Opciones de Deployment

1. **Vercel** (Frontend) + **Railway/Render** (Backend)
2. **Heroku** (Todo en uno)
3. **Docker** + **VPS** (DigitalOcean, AWS, etc.)
4. **Netlify** (Frontend) + **Fly.io** (Backend)

## 1. Deployment con Vercel + Railway

### Frontend en Vercel

1. **Preparar el Frontend:**

```bash
cd app
npm run build
```

2. **Deploy a Vercel:**

```bash
# Instala Vercel CLI
npm install -g vercel

# Deploy
cd app
vercel
```

O conecta tu repositorio GitHub con Vercel:
- Ve a [vercel.com](https://vercel.com)
- Importa tu proyecto desde GitHub
- Configura el root directory como `app`
- Agrega variables de entorno:
  - `VITE_API_URL`: URL de tu backend

### Backend en Railway

1. **Crear cuenta en Railway:**
   - Ve a [railway.app](https://railway.app)
   - Conecta con GitHub

2. **Crear nuevo proyecto:**
   - New Project → Deploy from GitHub
   - Selecciona tu repositorio
   - Configure root directory como `server`

3. **Agregar MongoDB:**
   - Add Plugin → MongoDB
   - Railway creará automáticamente una base de datos

4. **Variables de Entorno en Railway:**

```env
NODE_ENV=production
PORT=3000
MONGODB_URI=${{MongoDB.MONGO_URL}}
JWT_SECRET=tu_secret_seguro_de_produccion
JWT_EXPIRE=7d
OPENAI_API_KEY=sk-...
STRIPE_SECRET_KEY=sk_live_...
CORS_ORIGIN=https://tu-app.vercel.app
```

5. **Build y Start Scripts:**

Railway detectará automáticamente tu `package.json`. Asegúrate de tener:

```json
{
  "scripts": {
    "build": "tsc",
    "start": "node dist/index.js"
  }
}
```

## 2. Deployment con Docker

### Dockerfile para Backend

Crea `server/Dockerfile`:

```dockerfile
FROM node:18-alpine

WORKDIR /app

COPY package*.json ./
RUN npm ci --only=production

COPY . .
RUN npm run build

EXPOSE 3000

CMD ["npm", "start"]
```

### Dockerfile para Frontend

Crea `app/Dockerfile`:

```dockerfile
FROM node:18-alpine as builder

WORKDIR /app

COPY package*.json ./
RUN npm ci

COPY . .
RUN npm run build

FROM nginx:alpine
COPY --from=builder /app/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
```

### Docker Compose

Crea `docker-compose.yml` en la raíz:

```yaml
version: '3.8'

services:
  mongo:
    image: mongo:latest
    restart: always
    environment:
      MONGO_INITDB_ROOT_USERNAME: admin
      MONGO_INITDB_ROOT_PASSWORD: password
    ports:
      - "27017:27017"
    volumes:
      - mongo-data:/data/db

  backend:
    build: ./server
    restart: always
    ports:
      - "3000:3000"
    environment:
      NODE_ENV: production
      MONGODB_URI: mongodb://admin:password@mongo:27017/y_que_comemos?authSource=admin
      JWT_SECRET: ${JWT_SECRET}
      OPENAI_API_KEY: ${OPENAI_API_KEY}
      CORS_ORIGIN: http://localhost
    depends_on:
      - mongo

  frontend:
    build: ./app
    restart: always
    ports:
      - "80:80"
    depends_on:
      - backend

volumes:
  mongo-data:
```

### Ejecutar con Docker:

```bash
docker-compose up -d
```

## 3. Deployment en VPS (DigitalOcean, AWS EC2)

### 1. Crear Droplet/Instancia

- Ubuntu 22.04 LTS
- Al menos 2GB RAM
- Configura SSH keys

### 2. Conectar y Configurar Servidor

```bash
# Conectar vía SSH
ssh root@tu-ip

# Actualizar sistema
apt update && apt upgrade -y

# Instalar Node.js
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
apt install -y nodejs

# Instalar MongoDB
wget -qO - https://www.mongodb.org/static/pgp/server-6.0.asc | apt-key add -
echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu focal/mongodb-org/6.0 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-6.0.list
apt update
apt install -y mongodb-org
systemctl start mongod
systemctl enable mongod

# Instalar Nginx
apt install -y nginx
systemctl start nginx
systemctl enable nginx

# Instalar PM2
npm install -g pm2
```

### 3. Clonar y Configurar Proyecto

```bash
# Clonar repositorio
cd /var/www
git clone https://github.com/tu-usuario/y_que_comemos.git
cd y_que_comemos

# Instalar dependencias
npm install

# Configurar variables de entorno
cp server/.env.example server/.env
nano server/.env
# Edita las variables

# Build
cd app && npm run build
cd ../server && npm run build
```

### 4. Configurar PM2

```bash
# Iniciar backend con PM2
cd /var/www/y_que_comemos/server
pm2 start dist/index.js --name y-que-comemos-api

# Guardar configuración PM2
pm2 save
pm2 startup
```

### 5. Configurar Nginx

Crea `/etc/nginx/sites-available/y-que-comemos`:

```nginx
server {
    listen 80;
    server_name tu-dominio.com;

    # Frontend
    location / {
        root /var/www/y_que_comemos/app/dist;
        try_files $uri $uri/ /index.html;
    }

    # Backend API
    location /api {
        proxy_pass http://localhost:3000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }
}
```

Activar configuración:

```bash
ln -s /etc/nginx/sites-available/y-que-comemos /etc/nginx/sites-enabled/
nginx -t
systemctl reload nginx
```

### 6. Configurar HTTPS con Let's Encrypt

```bash
# Instalar Certbot
apt install -y certbot python3-certbot-nginx

# Obtener certificado
certbot --nginx -d tu-dominio.com

# Auto-renovación
certbot renew --dry-run
```

## Variables de Entorno en Producción

**Crítico**: Asegúrate de configurar estas variables en producción:

```env
# Backend
NODE_ENV=production
MONGODB_URI=<tu-mongodb-uri>
JWT_SECRET=<secret-seguro-minimo-32-caracteres>
OPENAI_API_KEY=sk-<tu-key>
STRIPE_SECRET_KEY=sk_live_<tu-key>
CORS_ORIGIN=https://tu-dominio.com

# Frontend
VITE_API_URL=https://tu-dominio.com/api
```

## Checklist de Producción

- [ ] Variables de entorno configuradas
- [ ] MongoDB con contraseña segura
- [ ] JWT_SECRET único y seguro
- [ ] HTTPS configurado
- [ ] CORS configurado correctamente
- [ ] Backup automático de base de datos
- [ ] Monitoreo (ej: PM2, Datadog)
- [ ] Logs configurados
- [ ] Rate limiting habilitado
- [ ] Helmet.js para seguridad
- [ ] Compresión gzip habilitada

## Monitoreo

### PM2 Monitoring

```bash
# Ver logs
pm2 logs

# Monitorear recursos
pm2 monit

# Reiniciar app
pm2 restart y-que-comemos-api
```

### Configurar Backup de MongoDB

```bash
# Script de backup
#!/bin/bash
BACKUP_DIR="/backups/mongodb"
DATE=$(date +%Y%m%d_%H%M%S)

mongodump --out $BACKUP_DIR/$DATE

# Mantener solo últimos 7 días
find $BACKUP_DIR -type d -mtime +7 -exec rm -rf {} +
```

Agrega al crontab:
```bash
crontab -e
# Backup diario a las 2 AM
0 2 * * * /path/to/backup-script.sh
```

## Actualización de la Aplicación

```bash
cd /var/www/y_que_comemos

# Pull cambios
git pull origin main

# Reinstalar dependencias si es necesario
npm install

# Rebuild
cd app && npm run build
cd ../server && npm run build

# Reiniciar backend
pm2 restart y-que-comemos-api

# Recargar Nginx (si cambió configuración frontend)
systemctl reload nginx
```

## Troubleshooting en Producción

### Ver logs de backend
```bash
pm2 logs y-que-comemos-api
```

### Ver logs de Nginx
```bash
tail -f /var/log/nginx/error.log
tail -f /var/log/nginx/access.log
```

### Verificar estado de MongoDB
```bash
systemctl status mongod
mongosh
```

## Costos Estimados

### Opción 1: Vercel + Railway
- Frontend (Vercel): **Gratis** (Hobby plan)
- Backend (Railway): **$5-10/mes**
- MongoDB Atlas: **Gratis** (M0 tier)
- **Total: ~$5-10/mes**

### Opción 2: VPS
- DigitalOcean Droplet (2GB): **$12/mes**
- **Total: $12/mes**

### Opción 3: Heroku
- Dyno + MongoDB: **$7-15/mes**

## Soporte

Para problemas de deployment:
- Revisa los logs
- Consulta esta guía
- Abre un issue en GitHub
