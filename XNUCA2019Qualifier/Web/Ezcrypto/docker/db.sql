SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;


CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


CREATE SEQUENCE public."Record_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public."Record_id_seq" OWNER TO ezcrypto;


SET default_tablespace = '';


SET default_with_oids = false;


CREATE TABLE public."Record" (
    id integer DEFAULT nextval('public."Record_id_seq"'::regclass) NOT NULL,
    username character varying(30) NOT NULL,
    secretroot text NOT NULL,
    "Nuser" text NOT NULL,
    "Euser" text NOT NULL,
    secretuser text NOT NULL,
    lowlimit double precision NOT NULL,
    uplimit double precision NOT NULL,
    message text NOT NULL,
    luky integer NOT NULL
);


ALTER TABLE public."Record" OWNER TO ezcrypto;


CREATE SEQUENCE public."User_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public."User_id_seq" OWNER TO ezcrypto;


CREATE TABLE public."User" (
    id integer DEFAULT nextval('public."User_id_seq"'::regclass) NOT NULL,
    username character varying(30) NOT NULL,
    password character varying(32) NOT NULL,
    times integer NOT NULL,
    "Nroot" text NOT NULL,
    "Eroot" text NOT NULL
);


ALTER TABLE public."User" OWNER TO ezcrypto;