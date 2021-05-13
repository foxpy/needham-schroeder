create table if not exists users(
    name text unique,
    id blob unique,
    key blob unique
);
